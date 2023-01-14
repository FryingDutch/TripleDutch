#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../headers/FileHandler.h"

namespace TDA
{
	std::string FileHandler::workDir{ "/" };

	void FileHandler::setWorkDir(const char* _path)
	{
		workDir = _path;
	}

	void FileHandler::createFile(const char* _path, const char* _msg)
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
	        std::cerr << "Error creating log at: " << _path << "\n";
	}

	void FileHandler::copyFile(const char* _sourcePath, const char* _destinationPath)
	{
		std::string fullPath{ workDir + _sourcePath };
		std::ifstream currentFile(fullPath.c_str());

		if (currentFile)
		{
			std::ofstream newFile(_destinationPath);

			if (newFile)
			{
				newFile << currentFile.rdbuf();
				newFile.close();
			}

			currentFile.close();
		}
	    	else 
	            std::cerr << "Error copying log at: " << _sourcePath << "\n";
	}

	std::string FileHandler::readFile(const char* _name)
	{
		std::string fullPath{ workDir + _name };
		std::ostringstream ss;
		std::string result;

		std::ifstream fileToRead(fullPath);
		if (fileToRead)
		{
			ss << fileToRead.rdbuf();		
			fileToRead.close();
			return ss.str();
		}

	    else 
	    {
	        std::cerr << "Error reading log\n";
	        return "";
	    }
	}

	void FileHandler::writeToFile(const char* _path, const char* _msg)
	{
		if (!checkFileExistence(_path))
		{
			createFile(_path, _msg);
			return;
		}

		std::ofstream writeFile(_path, std::ios_base::app | std::ios_base::out);

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

	bool FileHandler::checkFileExistence(const char* _name)
	{
		std::string fullPath{_name};
		std::ifstream readFiles(fullPath);

		if (readFiles)
		{
			readFiles.close();
			return true;
		}
		return false;
	}

	nlohmann::json FileHandler::readJSON(std::string _fileLocation)
	{
		std::ifstream jsonReader(_fileLocation);
		nlohmann::json jsonContents;
		
		if(jsonReader)
		{
			jsonReader >> jsonContents;
			jsonReader.close();
		}

		return jsonContents;
	}
}