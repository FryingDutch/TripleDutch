#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <vector>
#include <iostream>
#include "../headers/QueryBuilder.h"
#include "../headers/Logger.h"

namespace TDA
{
    std::unique_ptr<sql::Connection> QueryBuilder::getConnection(std::string host, std::string user, std::string password)
    {
        uint32_t errorCode = 0;
        uint32_t tries = 0;

        std::unique_ptr<sql::Connection> connection = nullptr;

        do{
            try{
                connection = std::unique_ptr<sql::Connection>(driver->connect("tcp://db", "root", "root"));
            } catch (sql::SQLException& exception) {
                errorCode = exception.getErrorCode();
                std::cout << "Error(" << exception.getErrorCode() << ") Message: " << exception.what();
                tries += 1;
            }
        } while (errorCode == ERROR_CODES::CANNOT_CONNECT && tries < 20);

        return connection;
    }

    QueryBuilder QueryBuilder::select(std::vector<std::string>columns = std::vector<std::string>())
    {
        this->selectStatement = "SELECT ";
        uint32_t numOfColumns = columns.size();
        if(numOfColumns == 0)
        {
            this->selectStatement += "*";
            return *this;
        };

        for(size_t i = 0; i < numOfColumns; i++)
        {
            this->selectStatement += columns[i] + ",";
        }

        if(this->selectStatement.size() > 1 && this->selectStatement[this->selectStatement.size() - 1] == ',')
        {
            this->selectStatement.pop_back();
        }

        return *this;
    }

    QueryBuilder QueryBuilder::from(std::string table)
    {
        this->tableStatement = " FROM " + table;
        return *this;
    }

    std::string QueryBuilder::getQuery()
    {
        return this->selectStatement + this->tableStatement;
    }

    std::vector<std::vector<std::string>> QueryBuilder::fetchAll()
    {
        std::unique_ptr<sql::Connection> connection = getConnection();
        if(!connection)
        {
            exit(1);
        }

        connection->setSchema("test");

        std::unique_ptr<sql::Statement> statement(connection->createStatement());
        std::unique_ptr<sql::ResultSet> result(statement->executeQuery(this->getQuery()));

        sql::ResultSetMetaData* resultMetadata(result->getMetaData());
        uint32_t numOfColumns = resultMetadata->getColumnCount();

        std::vector<std::vector<std::string>> queryResults;
        size_t row = 0;

        while (result->next())
        {
            queryResults.push_back(std::vector<std::string>());
            for(size_t column = 1; column <= numOfColumns; column++)
            {
                queryResults[row].push_back(result->getString(column));
            }
            row++;
        }
        return queryResults;
    }
}