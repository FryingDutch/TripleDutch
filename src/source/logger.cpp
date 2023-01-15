#include <sstream>
#include <fstream>
#include "../headers/Logger.h"
#include "../headers/FileHandler.h"

namespace TDA
{
    const std::string Logger::LOG_ROOT{FileHandler::workDir + "logs/"};
    const std::string Logger::SQL_LOG_LOCATION{Logger::LOG_ROOT + "sql_connector"}; // We leave the ".log", we append it later to make it more easy to make specific files  
    const std::string Logger::GENERAL_LOG_LOCATION{Logger::LOG_ROOT + "general"};

    void Logger::createLog(std::string _path, std::string _msg)
    {
        _path += ".log";
    	FileHandler::createFile(_path.c_str(), _msg.c_str());
    }

    void Logger::writeToLog(std::string _path, std::string _msg)
    {
        _path += ".log";
    	FileHandler::writeToFile(_path.c_str(), _msg.c_str());
    }

    // GENRAL LOGS / PRIVATE
    void Logger::createLogGeneral(std::string _msg, std::string extension)
    {
    	Logger::createLog(GENERAL_LOG_LOCATION + extension, _msg);
    }

    void Logger::writeToLogGeneral(std::string _msg, std::string extension)
    {
    	Logger::writeToLog(GENERAL_LOG_LOCATION + extension, _msg);
    }  

    // SQL Logs / PRIVATE
    void Logger::createLogSQL(std::string _msg, std::string extension)
    {
    	Logger::createLog(SQL_LOG_LOCATION + extension, _msg);
    }

    void Logger::writeToLogSQL(std::string _msg, std::string extension)
    {
    	Logger::writeToLog(SQL_LOG_LOCATION + extension, _msg);
    }

    // GENERAL LOGS / PUBLIC
    void Logger::General_Debug(std::string _msg)
    {
        static const std::string debug = "GENERAL_DEBUG: ";
        Logger::writeToLogGeneral(debug + _msg, "_debug");
    }

    void Logger::General_Info(std::string _msg)
    {
        static const std::string info = "GENERAL_INFO: ";
        Logger::writeToLogGeneral(info + _msg, "_info");
    }

    void Logger::General_Warning(std::string _msg)
    {
        static const std::string warning = "GENERAL_WARNING: ";
        Logger::writeToLogGeneral(warning + _msg, "_warning");
    }

    void Logger::General_Exception(std::string _errorMessage)
    {
        static const std::string critical = "GENERAL_EXCEPTION: ";
        Logger::writeToLogGeneral(critical + _errorMessage, "_exception");
    }  

    void Logger::General_Exception(std::string _errorMessage, uint32_t _errorCode)
    {
        const std::string critical = "GENERAL_EXCEPTION: error-code(" + std::to_string(_errorCode) + ") error-message: '" + _errorMessage + "'";
        Logger::writeToLogGeneral(critical + _errorMessage, "_exception");
    }  

    // SQL Logs / PUBLIC
    void Logger::SQL_Debug(std::string _msg)
    {
        static const std::string debug = "SQL_DEBUG: ";
        Logger::writeToLogSQL(debug + _msg, "_debug");
    }

    void Logger::SQL_Info(std::string _msg)
    {
        static const std::string info = "SQL_INFO: ";
        Logger::writeToLogSQL(info + _msg, "_info");
    }

    void Logger::SQL_Warning(std::string _msg)
    {
        static const std::string warning = "SQL_WARNING: ";
        Logger::writeToLogSQL(warning + _msg, "_warning");
    }

    void Logger::SQL_Exception(std::string _errorMessage)
    {
        static const std::string critical = "SQL_EXCEPTION: ";
        Logger::writeToLogSQL(critical + _errorMessage, "_exception");
    }

    void Logger::SQL_Exception(std::string _errorMessage, uint32_t _errorCode)
    {
        const std::string critical = "SQL_EXCEPTION: error-code(" + std::to_string(_errorCode) + ") error-message: '" + _errorMessage + "'";
        Logger::writeToLogSQL(critical + _errorMessage, "_exception");
    }
}