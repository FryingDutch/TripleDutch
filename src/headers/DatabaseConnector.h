#pragma once
#include <mysql_connection.h>
#include <cppconn/driver.h>

namespace TDA {

    class DatabaseConnector
    {
        private:
            static sql::Driver *m_pdriver;
            static std::string m_host;
            static std::string m_user;
            static std::string m_password;
            static std::string m_database;

            enum ERROR_CODES{
                CANNOT_CONNECT=2003
            };            

        public:
            static std::unique_ptr<sql::Connection> createConnection();
    };

}