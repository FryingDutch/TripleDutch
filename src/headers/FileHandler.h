#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace TDA
{
    class FileHandler
    {
    protected:
    	static std::string workDir;

    private:
    	static void setWorkDir(const char* _path);

	protected:
		static std::string readFile(const char* _name);
		static void createFile(const char* _name, const char* _msg = "");
		static void writeToFile(const char* _name, const char* _msg);    	
    	static void copyFile(const char* _sourcePath, const char* _destinationPath);
        static bool checkFileExistence(const char* _name);

	public:
		static nlohmann::json readJSON(std::string _fileName);
    }; 
}