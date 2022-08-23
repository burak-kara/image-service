#include "Server.h"

Server::Server(const ImageHelper &imageHelper) : imageHelper(imageHelper) {
    this->wsaData = {};
    this->iResult = {};
    this->serverSocket = INVALID_SOCKET;
    this->clientSocket = INVALID_SOCKET;
    this->result = nullptr;
    this->iSendResult = {};
    this->isSocketClient = false;
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
        try {
            this->get();
            this->processBuffer();
            this->processImage();
        } catch (const std::exception &e) {
            printf("%s\n", e.what());
            Server::sendError(e.what());
        }
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

void Server::processBuffer() {
    std::string query = getQuery();
    std::string operationPart = Server::splitOperation(query);
    Server::assignOperation(operationPart);
}

std::string Server::getQuery() {
    std::vector<std::string> buffer = Helper::split(Helper::toString(this->receivedBuffer), ' ');
    if (buffer.size() < 2) {
        throw std::logic_error("Invalid request. Please provide an url and an operation.");
    }
    this->isSocketClient = buffer.size() == 2;
    return buffer[1];
}

// TODO is valid URL
std::string Server::splitOperation(const std::string &urlPath) {
    std::vector<std::string> buffer = Helper::split(urlPath, URL_DELIMITER);
    if (buffer.size() < 2) {
        throw std::logic_error("Invalid request. Please provide a valid url and an operation.");
    }
    this->url = "https://" + buffer[1];
    return Server::getOperationPart(buffer[0]);
}

std::string Server::getOperationPart(const std::string &operationPart) {
    std::vector<std::string> buffer = Helper::split(operationPart, URL_PATH_DELIMITER);
    if (buffer.size() < 3) {
        throw std::logic_error("Invalid request. Please provide an operation.");
    }
    return buffer[2];
}

void Server::assignOperation(const std::string &operationPart) {
    std::vector<std::string> buffer = Helper::split(operationPart, OPERATION_DELIMITER);
    if (buffer.empty()) {
        throw std::logic_error("Invalid request. Please provide a valid operation.");
    } else if (buffer.size() == 1) {
        if (buffer[0] != GRAYSCALE_OPERATION) {
            throw std::logic_error("Invalid request. Please provide a valid operation.");
        }
        this->operation = buffer[0];
        this->operationParams = "";
    } else {
        this->operation = buffer[0];
        this->operationParams = buffer[1];
    }
}

// TODO image not found, wrong params, wrong param format, wrong operation
void Server::processImage() {
    try {
        Image image = imageHelper.getImage(this->url);
        std::vector<std::string> sizes = Helper::split(this->operationParams, OPERATION_SIZE_DELIMITER);
        if (this->operation == RESIZE_OPERATION) {
            Server::operateResize(image, sizes);
        } else if (this->operation == ROTATE_OPERATION) {
            Server::operateRotate(image, sizes);
        } else if (this->operation == GRAYSCALE_OPERATION) {
            Server::operateGrayScale(image);
        } else if (this->operation == CROP_OPERATION) {
            Server::operateCrop(image, sizes);
        } else {
            Server::sendError("Operation not found");
        }
        Server::sendImage(image);
    } catch (const std::exception &e) {
        Server::sendError(e.what());
    }
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

void Server::operateGrayScale(Image &image) {
    imageHelper.grayscale(image);
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

std::string Server::prepareResponse(const Blob &blob) const {
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: ";
    if (this->isSocketClient) {
        response += "image/jpeg\r\nContent-Length: " + std::to_string(blob.length());
        response += "\r\n\r\n" + blob.base64() + "\r\n__END__";
    } else {
        std::string body =
                R"(<html><body><div><p>Taken from wikpedia</p><img src="data:image/png;base64, )" + blob.base64() +
                R"(" alt="Wiki image" /></div></body></html>)";
        response += "text/html\r\nContent-Length: " + std::to_string(body.length()) + "\r\n\r\n" + body;
    }
    return response;
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

