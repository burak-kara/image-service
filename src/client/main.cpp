#include "Client.cpp"

int main(int argc, char *argv[]) {
    InitializeMagick(argv[0]);
    ImageHelper imageHelper(*argv);
    char* url = R"(GET /v1/rotate:90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg)";
    if (argc > 1) {
        url = argv[1];
    }
    Client client(imageHelper, url);
    client.create();
    client.sendInitBuffer();
    client.receive();
    client.shutdown();
}

// GET /v1/resize:640x360/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
// GET /v1/rotate:90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
// GET /v1/grayscale/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
// GET /v1/crop:100x100x90x90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
// GET /v1/WRONG:100x100x90x90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg
