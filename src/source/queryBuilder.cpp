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
        m_baseStatement = "";
        m_tableStatement = "";
        m_whereStatement = "";
        m_query = "";
    }

    QueryBuilder* QueryBuilder::Delete()
    {
        this->clearQuery(); // Every select should define the start of a new query, so we clear all previous entries
        m_baseStatement = "DELETE";

        return this;
    }

    QueryBuilder* QueryBuilder::update(std::string _table, std::vector<std::string> _columns, std::vector<std::string> _values)
    {
        this->clearQuery(); // Every select should define the start of a new query, so we clear all previous entries
        m_baseStatement = "UPDATE " + _table + " SET ";
        if(_columns.size() == _values.size())
        {
            for(size_t i = 0; i < _columns.size(); i++){
                m_baseStatement += "`" + _columns[i] + "` = " + (System::isDigit(_values[i]) ? std::string(_values[i]) : ("\'" + _values[i] + "\'")) + ",";
            }

            if(m_baseStatement.size() > 1 && m_baseStatement[m_baseStatement.size() - 1] == ','){
                m_baseStatement.pop_back();
            }
        }

        return this;
    }

    QueryBuilder* QueryBuilder::select(std::vector<std::string>_columns)
    {
        this->clearQuery(); // Every select should define the start of a new query, so we clear all previous entries
        m_baseStatement = "SELECT ";

        uint32_t numOfColumns = _columns.size();
        if(numOfColumns == 0)
        {
            m_baseStatement += "* ";
            return this;
        };

        for(size_t i = 0; i < numOfColumns; i++)
        {
            m_baseStatement += _columns[i] + ",";
        }

        if(m_baseStatement.size() > 1 && m_baseStatement[m_baseStatement.size() - 1] == ',')
        {
            m_baseStatement.pop_back();
        }

        return this;
    }

    QueryBuilder* QueryBuilder::insert(std::string _table, std::vector<std::string> _columns, std::vector<std::string> _values)
    {
        this->clearQuery(); // Every insert should define the start of a new query, so we clear all previous entries
        m_baseStatement = "INSERT INTO `" + _table + "` (";
        for(size_t i = 0; i < _columns.size(); i++){
            m_baseStatement += _columns[i] + ",";
        }
        if(m_baseStatement.size() > 1 && m_baseStatement[m_baseStatement.size() - 1] == ','){
            m_baseStatement.pop_back();
        }
        m_baseStatement += ") VALUES (";
        for(size_t i = 0; i < _columns.size() && i < _values.size(); i++){
            m_baseStatement += (System::isDigit(_values[i]) ? std::string(_values[i]) : ("\'" + _values[i] + "\'")) + ",";
        }
        if(m_baseStatement.size() > 1 && m_baseStatement[m_baseStatement.size() - 1] == ','){
            m_baseStatement.pop_back();
        }
        m_baseStatement += ")";

        return this;
    }

    QueryBuilder* QueryBuilder::from(std::string _table)
    {
        m_tableStatement = " FROM `" + _table + "`";
        return this;
    }

    QueryBuilder* QueryBuilder::where(std::string _stmt, std::vector<std::string> _values, bool _isOr)
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

        if(!_isOr){
            m_whereStatement += ((m_whereStatement.size() <= 0) ? " WHERE " : " AND ") + _stmt;
        } else {
            m_whereStatement += " OR " + _stmt;
        }
        
        return this;
    }

    QueryBuilder* QueryBuilder::And(std::string _stmt, std::vector<std::string> _values)
    {
        this->where(_stmt, _values, false);
        return this;
    }

    QueryBuilder* QueryBuilder::Or(std::string _stmt, std::vector<std::string> _values)
    {
        this->where(_stmt, _values, true);
        return this;
    }

    std::string QueryBuilder::getQuery()
    {
        std::string stmt = m_baseStatement + m_tableStatement + m_whereStatement + ";";
        return stmt;
    }

    std::vector<std::vector<std::string>> QueryBuilder::fetchAll()
    {
        static std::unique_ptr<sql::Connection> connection = DatabaseConnector::createConnection();
        std::vector<std::vector<std::string>> queryResults;

        if(!connection || !connection->isValid())
        {
            Logger::SQL_Info("Unable to connect to database. Resetting Connection");
            connection = DatabaseConnector::createConnection();

            if(!connection || !connection->isValid())
            {
                queryResults.push_back(std::vector<std::string>{{"ERROR_404"}});
                return queryResults;
            }
        }

        std::unique_ptr<sql::Statement> statement(connection->createStatement());
        std::unique_ptr<sql::ResultSet> result(statement->executeQuery(this->getQuery()));

        sql::ResultSetMetaData* resultMetadata(result->getMetaData());
        uint32_t numOfColumns = resultMetadata->getColumnCount();

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

    void QueryBuilder::execute()
    {
        std::unique_ptr<sql::Connection> connection = DatabaseConnector::createConnection();
        if(!connection)
        {
            Logger::SQL_Warning("Unable to connect to database");
        }

        std::unique_ptr<sql::Statement> statement(connection->createStatement());
        try{
            statement->executeQuery(this->getQuery());
        } catch (sql::SQLException& exception) {
            if(exception.what() != ""){ 
                Logger::SQL_Exception(exception.what(), exception.getErrorCode());
            }
        }
    }
}