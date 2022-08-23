#include "Server.h"

Server::Server(const ImageHelper &imageHelper) : imageHelper(imageHelper) {
    this->wsaData = {};
    this->iResult = {};
    this->serverSocket = INVALID_SOCKET;
    this->clientSocket = INVALID_SOCKET;
    this->result = nullptr;
    this->iSendResult = {};
}

void Server::create() {
    this->init();
    this->resolve();
    this->createSocket();
    this->bind();
    this->listen();
    this->accept();
    this->receive();
}

void Server::init() {
    this->iResult = WSAStartup(MAKEWORD(2, 2), &(this->wsaData));
    if (this->iResult != 0) {
        printf("WSAStartup failed with error: %d\n", this->iResult);
        exit(1);
    }

    ZeroMemory(&(this->hints), sizeof(this->hints));
    this->hints.ai_family = AF_INET;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_protocol = IPPROTO_TCP;
    this->hints.ai_flags = AI_PASSIVE;
}

void Server::resolve() {
    this->iResult = getaddrinfo(nullptr, PORT, &(this->hints), &(this->result));
    if (this->iResult != 0) {
        Server::cleanup("getaddrinfo");
    }
}

void Server::createSocket() {
    this->serverSocket = socket(this->result->ai_family, this->result->ai_socktype, this->result->ai_protocol);
    if (this->serverSocket == INVALID_SOCKET) {
        Server::freeAddrInfo();
        Server::cleanup("socket");
    }
}

void Server::bind() {
    this->iResult = ::bind(this->serverSocket, this->result->ai_addr, (int) this->result->ai_addrlen);
    if (this->iResult == SOCKET_ERROR) {
        Server::freeAddrInfo();
        Server::closeServerSocket();
        Server::cleanup("bind");
    }
    Server::freeAddrInfo();
}

void Server::listen() {
    this->iResult = ::listen(this->serverSocket, SOMAXCONN);
    if (this->iResult == SOCKET_ERROR) {
        Server::closeServerSocket();
        Server::cleanup("listen");
    }
}

void Server::accept() {
    this->clientSocket = ::accept(this->serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        Server::cleanup("accept");
    }
    Server::closeServerSocket();
}

void Server::receive() {
    do {
        this->get();
        this->processBuffer();
        this->processImage();
    } while (this->iResult > 0);
}

void Server::get() {
    this->iResult = recv(this->clientSocket, this->receivedBuffer, this->receivedBufferLength, 0);
    if (this->iResult > 0) {
        printf("Bytes received: %d\n", this->iResult);
    } else if (this->iResult == 0)
        printf("Connection closing...\n");
    else {
        Server::closeClientSocket();
        Server::cleanup("recv");
    }
}

// TODO handle no url case, or not founds
void Server::processBuffer() {
    std::vector<std::string> buffer = getBuffer();
    std::string urlPath = buffer[1];
    buffer = Helper::split(urlPath, URL_DELIMITER);
    std::string operationPart = buffer[0];
    this->url = "https://" + buffer[1];
    buffer = Helper::split(operationPart, URL_PATH_DELIMITER);
    buffer = Helper::split(buffer[2], OPERATION_DELIMITER);
    this->operation = buffer[0];
    this->operationParams = buffer[1];
}

std::vector<std::string> Server::getBuffer() {
    return Helper::split(Helper::toString(this->receivedBuffer), ' ');
}

// TODO image not found, wrong params, wrong param format, wrong operation
void Server::processImage() {
    Image image = imageHelper.getImage(this->url);
    std::vector<std::string> sizes = Helper::split(this->operationParams, OPERATION_SIZE_DELIMITER);
    if (this->operation == RESIZE_OPERATION) {
        Server::operateResize(image, sizes);
    } else if (this->operation == ROTATE_OPERATION) {
        Server::operateRotate(image, sizes);
    } else if (this->operation == GRAYSCALE_OPERATION) {
        Server::operateGrayScale(image, sizes);
    } else if (this->operation == CROP_OPERATION) {
        Server::operateCrop(image, sizes);
    } else {
        Server::sendError("Operation not found");
    }
    Server::sendImage(image);
}

void Server::operateResize(Image &image, std::vector<std::string> &sizes) {
    if (sizes.size() != 2) {
        Server::sendError("Wrong resize params!");
        return;
    } else {
        imageHelper.resize(image, this->operationParams + "!");
    }
}

void Server::operateRotate(Image &image, std::vector<std::string> &sizes) {
    if (sizes.size() != 1) {
        Server::sendError("Wrong rotate params!");
        return;
    } else {
        imageHelper.rotate(image, stoi(this->operationParams));
    }
}

void Server::operateGrayScale(Image &image, std::vector<std::string> &sizes) {
    if (!sizes.empty()) {
        Server::sendError("Wrong grayscale params!");
        return;
    } else {
        imageHelper.grayscale(image);
    }
}

void Server::operateCrop(Image &image, std::vector<std::string> &sizes) {
    if (sizes.size() != 2 && sizes.size() != 4) {
        Server::sendError("Wrong crop params!");
        return;
    } else if (sizes.size() == 2) {
        imageHelper.crop(image, this->operationParams + "!");
    } else {
        imageHelper.crop(image, Size(stoi(sizes[0]), stoi(sizes[1])), Size(stoi(sizes[2]), stoi(sizes[3])));
    }
}

// TODO get image extension from url
void Server::sendImage(Image &image) {
    Blob blob;
    image.magick("JPEG");
    image.write(&blob);
    std::string res = Server::prepareResponse(blob);
    Server::send(const_cast<char *>(res.c_str()));
}

std::string Server::prepareResponse(const Blob &blob) {
    return "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: " + std::to_string(blob.length()) +
           "\r\n\r\n" + blob.base64() + "\r\n__END__";
}

void Server::send(const char *response) {
    this->iSendResult = ::send(this->clientSocket, response, (int) strlen(response), 0);
    if (this->iSendResult == SOCKET_ERROR) {
        Server::closeClientSocket();
        Server::cleanup("send");
    }
    printf("Bytes sent: %d\n", this->iSendResult);
}

void Server::sendError(const std::string &message) {
    std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: " +
                           std::to_string(message.length()) + "\r\n\r\n" + message + "\r\n__END__";
    Server::send(const_cast<char *>(response.c_str()));
}

void Server::shutdown() {
    this->terminate();
}

void Server::terminate() {
    iResult = ::shutdown(clientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        Server::closeClientSocket();
        Server::cleanup("terminate");
    }
    Server::closeClientSocket();
    WSACleanup();
}

void Server::cleanup(const std::string &str) {
    printf("%s failed with error: %d\n", str.c_str(), WSAGetLastError());
    WSACleanup();
    exit(1);
}

void Server::closeServerSocket() const {
    ::closesocket(this->serverSocket);
}

void Server::closeClientSocket() const {
    ::closesocket(this->clientSocket);
}

void Server::freeAddrInfo() const {
    freeaddrinfo(this->result);
}

