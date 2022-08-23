#ifndef IMAGE_SERVICE_CLIENT_H
#define IMAGE_SERVICE_CLIENT_H
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <Magick++.h>
#include "image-helper/ImageHelper.cpp"
#include "helper/Helper.cpp"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define BUFFER_LENGTH 512
#define HOST "127.0.0.1"
#define PORT "8080"

using namespace Magick;

class Client {
public:
    explicit Client(const ImageHelper &imageHelper, const char *url);

    void create();

    void sendInitBuffer();

    void receive();

    void shutdown();

private:
    ImageHelper imageHelper;

    WSADATA wsaData{};
    SOCKET connectSocket;
    struct addrinfo *result, *ptr, hints{};
    const char *sendBuffer;
    int iResult{};
    char receivedBuffer[BUFFER_LENGTH]{};
    int receivedBufferLength = BUFFER_LENGTH;

    void init();

    void resolve();

    void tryConnect();

    void createSocket();

    bool connect();

    static bool isError(const std::string &response);

    static void handleError(const std::string &response);

    static bool isEndOfStream(const std::string &response);

    static std::string getBody(const std::string &response);

    void saveImage(const std::string &imageStr);

    static void cleanup(const std::string &str);

    void closeSocket() const;

    void freeAddrInfo() const;
};

#endif //IMAGE_SERVICE_CLIENT_H
