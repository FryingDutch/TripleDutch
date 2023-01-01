#include <iostream>
#include "../headers/Server.h"
#include "../headers/System.h"

namespace TDA
{
    //setting functions
    void System::errormsg(const char* message)
    {
        std::cerr << "[ERROR]: " << message << "! Terminating...\n";
    }

    bool System::isDigit(std::string str)
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