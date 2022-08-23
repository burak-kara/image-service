#include "server/Server.cpp"

using namespace std;

class Main {
public:
    Server server;

    explicit Main(char **argv, Server server);
};

Main::Main(char **argv, Server server) : server(server) {
    InitializeMagick(*argv);
    server.create();
    server.shutdown();
}

int main(int argc, char **argv) {
    Main main(argv, Server(ImageHelper(*argv)));
}

