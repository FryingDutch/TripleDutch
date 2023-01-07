#include <sstream>
#include <fstream>
#include "../headers/Logger.h"
#include "../headers/FileHandler.h"

namespace TDA
{
    const std::string Logger::logRoot{"logs/"};
    const std::string Logger::sqlLogLocation{FileHandler::workDir + Logger::logRoot + "sql_connector.log"};    

    void Logger::createLog(std::string _path, std::string _msg)
    {
    	FileHandler::createFile(_path.c_str(), _msg.c_str());
    }

    void Logger::writeToLog(std::string _path, std::string _msg)
    {
    	FileHandler::writeToFile(_path.c_str(), _msg.c_str());
    }

    std::string Logger::readLog(std::string _name)
    {
    	return FileHandler::readFile(_name.c_str());
    }

    // SQL Logs / PRIVATE

    void Logger::createLogSQL(std::string _msg)
    {
    	createLog(sqlLogLocation, _msg);
    }

    void Logger::writeToLogSQL(std::string _msg)
    {
    	writeToLog(sqlLogLocation, _msg);
    }

    std::string Logger::readLogSQL()
    {
    	return readLog(sqlLogLocation);
    }

    // SQL Logs / PUBLIC
    void Logger::SQL_Debug(std::string _msg)
    {
        static std::string debug = "SQL_DEBUG: ";
        createLogSQL(debug + _msg);
    }

    void Logger::SQL_Info(std::string _msg)
    {
        static std::string info = "SQL_INFO: ";
        createLogSQL(info + _msg);
    }

    void Logger::SQL_Warning(std::string _msg)
    {
        static std::string warning = "SQL_WARNING: ";
        createLogSQL(warning + _msg);
    }

    void Logger::SQL_Exception(std::string _errorMessage)
    {
        static std::string critical = "SQL_EXCEPTION: ";
        createLogSQL(critical + _errorMessage);
    }
}