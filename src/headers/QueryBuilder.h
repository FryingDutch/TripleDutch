#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <vector>
#include <iostream>

namespace TDA
{
    class QueryBuilder
    {
        private:
            sql::Driver *driver;
            std::string selectStatement;
            std::string tableStatement;
            std::string query;

            uint32_t resultColumnCount;

            enum ERROR_CODES{
                CANNOT_CONNECT=2003
            };
            
        private:
            std::unique_ptr<sql::Connection> getConnection(std::string host, std::string user, std::string password);

        public:
            QueryBuilder select(std::vector<std::string>columns);
            QueryBuilder from(std::string table);
            std::string getQuery();
            std::vector<std::vector<std::string>> fetchAll();

        public:
            QueryBuilder(): driver(get_driver_instance()), selectStatement(std::string{""}), tableStatement(std::string{""}), query(std::string{""}), resultColumnCount(int{0}){}
    };
}