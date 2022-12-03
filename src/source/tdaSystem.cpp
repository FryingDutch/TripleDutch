#include <iostream>
#include "../headers/TDAServer.h"
#include "../headers/TDASystem.h"

namespace TDA
{
    //setting functions
    void TDASystem::errormsg(const char* message)
    {
        std::cerr << "[ERROR]: " << message << "! Terminating...\n";
    }

    bool TDASystem::isDigit(std::string str)
    {
        if(str.length() < 1)
        {
            return false;
        }

        for (size_t i = 0; i < str.length(); i++)
        {
            if (!std::isdigit(str[i]))
            {
                return false;
            }
        }
        return true;
    }
}