#include "../headers/Server.h"
#include "../headers/QueryBuilder.h"

int main(int argc, char* argv[])
{
    sleep(2); // This is to prevent the app from starting before the sql image is ready for connections

    TDA::Server::startup();
    return 0;
}