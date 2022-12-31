#include "../headers/TDAServer.h"
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

int main(int argc, char* argv[])
{
    // Load the MySQL Connector/C++ driver
    sql::Driver *driver = get_driver_instance();

    // Connect to the database
    std::unique_ptr<sql::Connection> conn(driver->connect("tcp://db", "root", "root"));   

    TDA::TDAServer::startup();
    return 0;
}