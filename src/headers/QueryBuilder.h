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
            std::string m_selectStatement;
            std::string m_tableStatement;
            std::string m_whereStatement;
            std::string m_query;

            uint32_t m_resultColumnCount;
            
        private:
            void clearQuery();

        public:
            QueryBuilder select(std::vector<std::string>_columns = std::vector<std::string>());
            QueryBuilder from(std::string _table);
            QueryBuilder where(std::string _stmt, std::vector<std::string> _values = {}, bool _isAnd = false);
            std::string getQuery();
            std::vector<std::vector<std::string>> fetchAll();
            QueryBuilder And(std::string _stmt, std::vector<std::string> _values = {});

        public:
            QueryBuilder(): m_selectStatement(std::string{""}), m_tableStatement(std::string{""}), m_query(std::string{""}), m_resultColumnCount(int{0}){}
    };
}