#include <sstream>
#include <fstream>
#include "Logger.h"

std::string Logger::workDir{ "/" };
std::ifstream Logger::readFiles;
std::ofstream Logger::writefiles;

void Logger::setWorkDir(const char* _path)
{
	workDir = _path;
}

bool Logger::checkLogExistence(const char* _name)
{
	std::string fullPath{ Logger::workDir + _name };

	readFiles.open(fullPath);
	if (readFiles)
	{
		readFiles.close();
		return true;
	}
	return false;
}

void Logger::createLog(const char* _path, const char* _msg)
{
	std::string fullPath{ workDir + _path };

	std::ofstream newLog(fullPath);
	if (newLog)
	{
		static const char* emptyString = "";
		if (_msg != emptyString)
			newLog << _msg;
		newLog.close();
	}
	else 
        std::cerr << "Error creating log\n";
}

void Logger::copyLog(const char* _sourcePath, const char* _destinationPath)
{
	std::string fullPath{ workDir + _sourcePath };
	std::ifstream log(fullPath.c_str());

	if (log)
	{
		std::ofstream copyLog(_destinationPath);

		if (copyLog)
		{
			copyLog << log.rdbuf();
			copyLog.close();
		}

		log.close();
	}
    	else 
            std::cerr << "Error copying log\n";
}

std::string Logger::readLog(const char* _name)
{
	std::string fullPath{ workDir + _name };
	std::ostringstream ss;
	std::string result;

	std::ifstream logToRead(fullPath);
	if (logToRead)
	{
		ss << logToRead.rdbuf();		
		logToRead.close();
		return ss.str();
	}

    else 
    {
        std::cerr << "Error reading log\n";
        return "";
    }
}

void Logger::writeToLog(const char* _name, const char* _msg)
{
	std::string fullPath{ workDir + _name };

	if (checkLogExistence(fullPath.c_str()))
	{
		std::ofstream writeFile(fullPath);

		if (writeFile)
		{
			writeFile << _msg << "\n";
			writeFile.close();
		}
		else 
			std::cerr << "Error writing to log\n";
	}
	else 
		std::cerr << "File does not exist\n";
}