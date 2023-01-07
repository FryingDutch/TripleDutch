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
        public:
            static const std::string logRoot;
            static const std::string sqlLogLocation;

        private:
            static void createLog(std::string _path, std::string _msg = "");
            static void writeToLog(std::string _path, std::string _msg);
            static std::string readLog(std::string _name);  

            static void createLogSQL(std::string _msg = "");
            static void writeToLogSQL(std::string _msg);
            static std::string readLogSQL();
        
        public:
            static void SQL_Info(std::string _msg);
            static void SQL_Warning(std::string _msg);
            static void SQL_Debug(std::string _msg);
            static void SQL_Exception(std::string _msg);
    };
}