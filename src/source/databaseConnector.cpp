#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <vector>
#include <iostream>
#include "../headers/DatabaseConnector.h"
#include "../headers/FileHandler.h"
#include "../headers/System.h"
#include "../headers/Logger.h"

namespace TDA
{
    sql::Driver* DatabaseConnector::m_pdriver = get_driver_instance();
    std::string DatabaseConnector::m_host = "tcp://" + std::string(System::getEnvironmentVariables()["tripledutch"]["database"]["host"]);
    std::string DatabaseConnector::m_user = System::getEnvironmentVariables()["tripledutch"]["database"]["user"];
    std::string DatabaseConnector::m_password = System::getEnvironmentVariables()["tripledutch"]["database"]["password"];
    std::string DatabaseConnector::m_database = System::getEnvironmentVariables()["tripledutch"]["database"]["db_name"];

    std::unique_ptr<sql::Connection> DatabaseConnector::createConnection()
    {
        uint32_t errorCode = 0;
        uint32_t tries = 0;

        std::unique_ptr<sql::Connection> connection = nullptr;

        do{
            try{
                connection = std::unique_ptr<sql::Connection>(m_pdriver->connect(m_host, m_user, m_password));
                connection->setSchema(m_database);
            } catch (sql::SQLException& exception) {
                errorCode = exception.getErrorCode();
                Logger::SQL_Exception(exception.what());
                tries += 1;
            }
        } while (errorCode == ERROR_CODES::CANNOT_CONNECT && tries < 20);

        return connection;
    }
}