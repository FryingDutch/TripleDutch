#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <vector>
#include <iostream>
#include "../headers/QueryBuilder.h"
#include "../headers/DatabaseConnector.h"
#include "../headers/Logger.h"
#include "../headers/System.h"

namespace TDA
{
    void QueryBuilder::clearQuery()
    {
        m_selectStatement = "";
        m_tableStatement = "";
        m_whereStatement = "";
        m_query = "";
    }

    QueryBuilder QueryBuilder::select(std::vector<std::string>_columns)
    {
        this->clearQuery(); // Every select should define the start of a new query, so we clear all previous entries
        m_selectStatement = "SELECT ";

        uint32_t numOfColumns = _columns.size();
        if(numOfColumns == 0)
        {
            m_selectStatement += "*";
            return *this;
        };

        for(size_t i = 0; i < numOfColumns; i++)
        {
            m_selectStatement += _columns[i] + ",";
        }

        if(m_selectStatement.size() > 1 && m_selectStatement[m_selectStatement.size() - 1] == ',')
        {
            m_selectStatement.pop_back();
        }

        return *this;
    }

    QueryBuilder QueryBuilder::from(std::string _table)
    {
        m_tableStatement = " FROM " + _table;
        return *this;
    }

    QueryBuilder QueryBuilder::where(std::string _stmt, std::vector<std::string> _values, bool _isAnd)
    {
        if(m_whereStatement.size() == 0 || _isAnd)
        {
            if(_values.size() > 0)
            {
                uint32_t numOfReplacements = 0;
                for(size_t i = 0; i < _stmt.size() && numOfReplacements < _values.size(); i++)
                {
                    if(_stmt[i] == '?')
                    {
                        std::string value = _values[numOfReplacements];
                        if(!System::isDigit(value))
                        {
                            value = ("\'" + value + "\'");
                        }
                        _stmt.replace(i, 1, value);
                        numOfReplacements++;
                    }
                }
            }
            m_whereStatement = (_isAnd ? m_whereStatement + " AND " : " WHERE ") + _stmt;
        }

        return *this;
    }

    QueryBuilder QueryBuilder::And(std::string _stmt, std::vector<std::string> _values)
    {
        this->where(_stmt, _values, true);

        return *this;
    }

    std::string QueryBuilder::getQuery()
    {
        std::string stmt = m_selectStatement + m_tableStatement + m_whereStatement;
        Logger::SQL_Debug(stmt);
        return m_selectStatement + m_tableStatement + m_whereStatement;
    }

    std::vector<std::vector<std::string>> QueryBuilder::fetchAll()
    {
        std::unique_ptr<sql::Connection> connection = DatabaseConnector::createConnection();
        if(!connection)
        {
            Logger::SQL_Info("Unable to connect to database");
        }

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