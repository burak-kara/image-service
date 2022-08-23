#include "Client.cpp"

int main(int argc, char *argv[]) {
    InitializeMagick(argv[0]);
    ImageHelper imageHelper(*argv);
    char* url = R"(/v1/resize:645x360/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg)";
    if (argc > 1) {
        url = argv[1];
    }
    Client client(imageHelper, url);
    client.create();
    client.sendInitBuffer();
    client.receive();
    client.shutdown();
}

// /v1/resize:640x360/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
// /v1/rotate:90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
// /v1/grayscale/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
// /v1/crop:100x100x90x90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
// /v1/WRONG:100x100x90x90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
