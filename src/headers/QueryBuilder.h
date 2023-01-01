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
            std::vector<std::string> selectVector;

            enum ERROR_CODES{
                CANNOT_CONNECT=2003
            };
            
        private:
            std::unique_ptr<sql::Connection> getConnection(std::string host = "tcp//db", std::string user = "root", std::string password = "root")
            {
                return std::unique_ptr<sql::Connection>(driver->connect("tcp://db", "root", "root"));
            }

        public:
            QueryBuilder select(std::vector<std::string>columns = std::vector<std::string>())
            {
                this->selectVector = columns;
                return *this;
            }

            void doTest()
            {
                uint32_t errorCode = 0;
                uint32_t tries = 0;

                std::unique_ptr<sql::Connection> conn = nullptr;
                do{
                    try{
                        conn = this->getConnection();
                    } catch (sql::SQLException& exception) {
                        errorCode = exception.getErrorCode();
                        std::cout << "Error(" << exception.getErrorCode() << ") Message: " << exception.what();
                        tries += 1;
                    }
                } while (errorCode == ERROR_CODES::CANNOT_CONNECT && tries < 20);

                if(tries >= 20){
                    exit(1);
                }

                conn->setSchema("test");

                std::unique_ptr<sql::Statement> stmt(conn->createStatement());
                std::string query = "SELECT ";
                this->select(std::vector<std::string>{"id", "name"});

                for(size_t i = 0; i < this->selectVector.size(); i++)
                {
                    query += this->selectVector[i] + ",";
                }

                if(query.size() > 1 && query[query.size() - 1] == ',')
                {
                    query.pop_back();
                }

                query += " FROM test";

                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

                while (res->next()) 
                {
                    std::cout << "Column 1: " << res->getString(1) << std::endl;
                    std::cout << "Column 2: " << res->getString(2) << std::endl;
                }
            }

        public:
            QueryBuilder(): driver(get_driver_instance()), selectVector(std::vector<std::string>()){}
    };
}