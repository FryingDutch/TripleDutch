#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include "../headers/FileHandler.h"

namespace TDA
{
    class Logger : public FileHandler
    {
        private:
            static const std::string LOG_ROOT;
            static const std::string SQL_LOG_LOCATION;
            static const std::string GENERAL_LOG_LOCATION;

        private:
            static void createLog(std::string _path, std::string _msg = "");
            static void writeToLog(std::string _path, std::string _msg);

            static void createLogGeneral(std::string _msg = "", std::string _extension = "");
            static void writeToLogGeneral(std::string _msg, std::string _extension = "");

            static void createLogSQL(std::string _msg = "", std::string _extension = "");
            static void writeToLogSQL(std::string _msg, std::string _extension = "");
        
        public:
            static void General_Info(std::string _msg);
            static void General_Warning(std::string _msg);
            static void General_Debug(std::string _msg);
            static void General_Exception(std::string _msg);
            static void General_Exception(std::string _msg, uint32_t _errorCode);

            static void SQL_Info(std::string _msg);
            static void SQL_Warning(std::string _msg);
            static void SQL_Debug(std::string _msg);
            static void SQL_Exception(std::string _msg);
            static void SQL_Exception(std::string _msg, uint32_t _errorCode);
    };
}