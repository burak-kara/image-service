#include "Client.cpp"

int main(int argc, char **argv) {
    InitializeMagick(*argv);
    ImageHelper imageHelper(*argv);
    Client client(imageHelper);
    client.create();
    client.sendInitBuffer();
    client.receive();
    client.shutdown();
}
