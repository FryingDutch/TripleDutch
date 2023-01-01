#define CROW_MAIN 
#define CROW_ENABLE_SSL
#include "crow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <optional>
#include "../headers/Server.h"
#include "../headers/QueryBuilder.h"

namespace TDA
{
    //runtime functions
    void Server::startup()
    {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/status")
            ([&](const crow::request& req) 
            {
                TDA::QueryBuilder qb;
                std::vector<std::string>columns{"id", "name"};
                std::vector<std::vector<std::string>> results = qb.select(columns).from("test").fetchAll();

                crow::json::wvalue jsonResult;
                for(size_t row = 0; row < results.size(); row++)
                {
                    std::string rowName = "row" + std::to_string(row);
                    jsonResult[rowName] = results[row];
                }

                return crow::response(200, jsonResult);
        });

        CROW_ROUTE(app, "/getlock")
            ([&](const crow::request& req)
        {
            crow::json::wvalue x;
            x["servername"] = "Helo world!";
            return crow::response(200, x);
        });

        // Releasing the lock
        CROW_ROUTE(app, "/releaselock").methods("DELETE"_method)
            ([&](const crow::request& req) {
            
            crow::json::wvalue x;
            x["servername"] = "Helo world!";
            return crow::response(200, x);
        });

        // configure the app instance with given parameters
        app.port(8001).server_name("Noice");

        app.run();
    }
}