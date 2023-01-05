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
                connection = std::unique_ptr<sql::Connection>(driver->connect(host, user, password));
            } catch (sql::SQLException& exception) {
                errorCode = exception.getErrorCode();
                std::cout << "Error(" << exception.getErrorCode() << ") Message: " << exception.what();
                tries += 1;
            }
        } while (errorCode == ERROR_CODES::CANNOT_CONNECT && tries < 20);

        return connection;
    }

    void QueryBuilder::clearQuery()
    {
        this->selectStatement = "";
        this->tableStatement = "";
        this->whereStatement = "";
        this->query = "";
    }

    QueryBuilder QueryBuilder::select(std::vector<std::string>columns)
    {
        this->clearQuery(); // Every select should define the start of a new query, so we clear all previous entries
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

    QueryBuilder QueryBuilder::where(std::string _whereStatement, std::vector<std::string> values)
    {
        if(this->whereStatement.size() == 0)
        {
            if(values.size() > 0)
            {
                std::vector<uint32_t> replacementIndexes = {};
                for(size_t i = 0; i < _whereStatement.size(); i++)
                {
                    if(_whereStatement[i] == '?')
                    {
                        whereStatement.replace(i, i, values[i]);
                    }
                }
            }
            this->whereStatement = " WHERE " + _whereStatement;
        }

        return *this;
    }

    std::string QueryBuilder::getQuery()
    {
        std::string stmt = this->selectStatement + this->tableStatement + this->whereStatement;
        Logger::writeToLog(Logger::sqlLogLocation.c_str(), stmt.c_str());
        return this->selectStatement + this->tableStatement + this->whereStatement;
    }

    std::vector<std::vector<std::string>> QueryBuilder::fetchAll()
    {
        std::unique_ptr<sql::Connection> connection = getConnection();
        if(!connection)
        {
            Logger::writeToLog(Logger::sqlLogLocation.c_str(), "Unable to connect to database");
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