#include <sstream>
#include <fstream>
#include "../headers/Logger.h"

namespace TDA
{
	std::string Logger::workDir{ "/logs/" };
	std::ifstream Logger::readFiles;
	std::ofstream Logger::writefiles;
	const std::string Logger::sqlLogLocation{Logger::workDir + "sql.log"};

	void Logger::setWorkDir(const char* _path)
	{
		workDir = _path;
	}

	void Logger::createLog(const char* _path, const char* _msg)
	{
		std::ofstream newLog(_path);
		if (newLog)
		{
			static const char* emptyString = "";
			if (_msg != emptyString)
				newLog << _msg << "\n";
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

	void Logger::writeToLog(const char* _path, const char* _msg)
	{
		if (!checkLogExistence(_path))
		{
			createLog(_path, _msg);
			return;
		}

		std::ofstream writeFile(_path, std::ios_base::app);

		if (writeFile)
		{
			writeFile << _msg << "\n";
			writeFile.close();
		}

		else
		{
			std::cerr << "Error writing to log\n";
		} 
	}

	bool Logger::checkLogExistence(const char* _name)
	{
		std::string fullPath{_name};

		readFiles.open(fullPath);
		if (readFiles)
		{
			readFiles.close();
			return true;
		}
		return false;
	}
}