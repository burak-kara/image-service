#undef UNICODE

#ifndef IMAGE_SERVICE_SERVER_H
#define IMAGE_SERVICE_SERVER_H

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <Magick++.h>
#include "image-helper/ImageHelper.cpp"
#include "size/Size.cpp"
#include "helper/Helper.cpp"

#pragma comment (lib, "Ws2_32.lib")

#define BUFFER_LENGTH 2048
#define PORT "8080"
#define URL_DELIMITER "url:"
#define URL_PATH_DELIMITER "/"
#define OPERATION_DELIMITER ":"
#define OPERATION_SIZE_DELIMITER "x"

#define RESIZE_OPERATION "resize"
#define CROP_OPERATION "crop"
#define GRAYSCALE_OPERATION "grayscale"
#define ROTATE_OPERATION "rotate"

using namespace Magick;

class Server {
public:
    explicit Server(const ImageHelper &imageHelper);

    void create();

    void shutdown();

private:
    ImageHelper imageHelper;

    WSADATA wsaData{};
    int iResult;

    SOCKET serverSocket;
    SOCKET clientSocket;

    struct addrinfo *result;
    struct addrinfo hints{};

    int iSendResult;
    char receivedBuffer[BUFFER_LENGTH]{};
    int receivedBufferLength = BUFFER_LENGTH;

    std::string url;
    std::string operation;
    std::string operationParams;
    bool isSocketClient;

    void init();

    void resolve();

    void createSocket();

    void bind();

    void listen();

    void accept();

    void get();

    void receive();

    void processBuffer();

    void processImage();

    std::string getQuery();

    std::string splitOperation(const std::string &urlPath);

    static std::string getOperationPart(const std::string &operationPart);

    void assignOperation(const std::string &operationPart);

    void operateResize(Image &image, std::vector<std::string> &sizes);

    void operateRotate(Image &image, std::vector<std::string> &sizes);

    void operateGrayScale(Image &image);

    void operateCrop(Image &image, std::vector<std::string> &sizes);

    void sendImage(Image &image);

    std::string prepareResponse(const Blob &blob) const;

    void send(const char *response);

    void sendError(const std::string &message);

    void terminate();

    void closeServerSocket() const;

    void closeClientSocket() const;

    void freeAddrInfo() const;

    static void cleanup(const std::string &str);
};

#endif //IMAGE_SERVICE_SERVER_H
