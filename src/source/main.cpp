#define CROW_MAIN 
#define CROW_ENABLE_SSL
#include <iostream>
#include "../headers/TDAServer.h"

int main(int argc, char* argv[])
{
    TDA::TDAServer::startup();
}