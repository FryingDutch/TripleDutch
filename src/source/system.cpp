#include <iostream>
#include <typeinfo>
#include <nlohmann/json.hpp>
#include "../headers/Server.h"
#include "../headers/System.h"
#include "../headers/FileHandler.h"

namespace TDA
{
    nlohmann::json System::environmentVariables = FileHandler::readJSON("/env.json");
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

    // TODO: Find a way to return only the key and its value which is requested
    nlohmann::json System::getEnvironmentVariables(std::vector<std::string>_jsonKeys)
    {   
        return environmentVariables;

        // if(_jsonKeys.size() <= 0)
        // {
        //     return environmentVariables;
        // }

        // nlohmann::json resultJson;
        // for(size_t i = 0; i < _jsonKeys.size(); i++)
        // {
        //     nlohmann::json::iterator it = environmentVariables.find(_jsonKeys[i]);
        //     if(it != environmentVariables.end())
        //     {
        //         resultJson[_jsonKeys[i]] = it.value();
        //     }
        // }
        // return resultJson;
    }
}