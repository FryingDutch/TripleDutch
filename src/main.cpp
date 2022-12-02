#define CROW_MAIN 
#define CROW_ENABLE_SSL
#include <iostream>
#include "crow.h"

int main(int argc, char* argv[])
{
    while(true)
    {
        std::cout << "Hello World\n my friend";
    }
    return 0;
}