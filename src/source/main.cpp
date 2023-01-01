#include "../headers/Server.h"
#include "../headers/QueryBuilder.h"
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

int main(int argc, char* argv[])
{
    TDA::QueryBuilder qb;
    qb.doTest();

    TDA::Server::startup();
    return 1;
}