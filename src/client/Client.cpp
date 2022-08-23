#include "Client.h"

Client::Client(const ImageHelper &imageHelper, const char *url) : imageHelper(imageHelper), sendBuffer(url) {
    this->connectSocket = INVALID_SOCKET;
    this->result = nullptr;
    this->ptr = nullptr;
}

void Client::create() {
    this->init();
    this->resolve();
    this->tryConnect();
}

void Client::init() {
    this->iResult = WSAStartup(MAKEWORD(2, 2), &(this->wsaData));
    if (this->iResult != 0) {
        printf("WSAStartup failed with error: %d\n", this->iResult);
        exit(1);
    }

    ZeroMemory(&(this->hints), sizeof(this->hints));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_protocol = IPPROTO_TCP;
}

void Client::resolve() {
    this->iResult = getaddrinfo(HOST, PORT, &(this->hints), &(this->result));
    if (this->iResult != 0) {
        Client::cleanup("getaddrinfo");
    }
}

void Client::tryConnect() {
    for (this->ptr = this->result; this->ptr != nullptr; this->ptr = this->ptr->ai_next) {
        this->createSocket();
        const bool isConnected = this->connect();
        if (!isConnected) {
            break;
        }
    }
    Client::freeAddrInfo();
    if (this->connectSocket == INVALID_SOCKET) {
        Client::cleanup("Unable");
    }
}

void Client::createSocket() {
    this->connectSocket = socket(this->ptr->ai_family, this->ptr->ai_socktype, this->ptr->ai_protocol);
    if (this->connectSocket == INVALID_SOCKET) {
        Client::cleanup("socket");
    }
}

bool Client::connect() {
    this->iResult = ::connect(this->connectSocket, this->ptr->ai_addr, (int) this->ptr->ai_addrlen);
    if (this->iResult == SOCKET_ERROR) {
        closesocket(this->connectSocket);
        this->connectSocket = INVALID_SOCKET;
        return true;
    }
    return false;
}

void Client::sendInitBuffer() {
    this->iResult = send(this->connectSocket, this->sendBuffer, (int) strlen(this->sendBuffer), 0);
    if (this->iResult == SOCKET_ERROR) {
        Client::closeSocket();
        Client::cleanup("send");
    }
    printf("Bytes Sent: %d\n", this->iResult);
}

void Client::shutdown() {
    this->iResult = ::shutdown(this->connectSocket, SD_SEND);
    if (this->iResult == SOCKET_ERROR) {
        Client::closeSocket();
        Client::cleanup("shutdown");
    }
}

void Client::receive() {
    int bytesReceived = 0;
    std::string response;
    do {
        this->iResult = recv(this->connectSocket, this->receivedBuffer, this->receivedBufferLength, 0);
        if (this->iResult > 0) {
            if (Client::isError(this->receivedBuffer)) {
                Client::handleError(this->receivedBuffer);
            } else {
                response.append(this->receivedBuffer);
                bytesReceived += this->receivedBufferLength;
                if (Client::isEndOfStream(response)) {
                    const std::string &imageStr = Client::getBody(response);
                    Client::saveImage(imageStr);
                }
            }
        } else if (this->iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
    } while (this->iResult > 0);
}

bool Client::isError(const std::string &response) {
    return response.find("HTTP/1.1 404 Not Found") != std::string::npos;
}

void Client::handleError(const std::string &response) {
    const std::string &body = Client::getBody(response);
    std::cout << body << std::endl;
}

bool Client::isEndOfStream(const std::string &response) {
    return response.find("\r\n__END__") != std::string::npos;
}

std::string Client::getBody(const std::string &response) {
    std::vector<std::string> buffer = Helper::split(response, "\r\n__END__");
    buffer = Helper::split(buffer[0], "\r\n\r\n");
    return buffer[1];
}

void Client::saveImage(const std::string &imageStr) {
    Blob blob;
    blob.base64(imageStr);
    Image image = imageHelper.setImage(blob);
    imageHelper.writeImage(image, "..\\out.jpg");
}

void Client::cleanup(const std::string &str) {
    std::cout << "Connected to server" << std::endl;
    std::printf("%s failed with error: %d\n", str.c_str(), WSAGetLastError());
    WSACleanup();
    exit(1);
}

void Client::closeSocket() const {
    ::closesocket(this->connectSocket);
}

void Client::freeAddrInfo() const {
    freeaddrinfo(this->result);
}
