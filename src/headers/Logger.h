#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

namespace TDA
{
    struct Logger
    {
    private:
    	static std::string workDir;
    	static std::ifstream readFiles;
    	static std::ofstream writefiles;

    public:
        static const std::string sqlLogLocation;

    public:
    	static void setWorkDir(const char* _path);
    	static void createLog(const char* _name, const char* _msg = "");
    	static void copyLog(const char* _sourcePath, const char* _destinationPath);
    	static std::string readLog(const char* _name);
    	static void writeToLog(const char* _name, const char* _msg);
    	static void removeLog(const char* _name);
        static bool checkLogExistence(const char* _name);
    }; 
}