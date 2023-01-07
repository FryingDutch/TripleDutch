#define CROW_MAIN 
#define CROW_ENABLE_SSL
#include "crow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <optional>
#include <nlohmann/json.hpp>
#include "../headers/System.h"
#include "../headers/Server.h"
#include "../headers/QueryBuilder.h"
#include "../headers/Logger.h"

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

                nlohmann::json resultJson;
                resultJson["status"] = "unknown";
                uint32_t responseCode = 200;

                if (req.url_params.get("auth") == nullptr)
                {
                    resultJson["status"] = "no api key";
                    responseCode = 400;
                    return crow::response(responseCode, resultJson.dump());
                }

                std::vector<std::string>columns{"api_key", "lock_name"};

                resultJson["servername"] = System::getEnvironmentVariables()["tripledutch"]["system"]["server_name"];
                resultJson["locks"] = nlohmann::json{};

                try{
                    nlohmann::json result = qb.select(columns).from("all_locks").where("api_key = ?",{req.url_params.get("auth")}).fetchAll();
                    resultJson["locks"] = result;
                    resultJson["status"] = "ok";
                    resultJson["error_message"] = "";
                    responseCode = 200;
                } catch (sql::SQLException& exception) {
                    Logger::SQL_Exception(exception.what());
                    resultJson["locks"] = nlohmann::json{};
                    resultJson["status"] = "bad";
                    resultJson["error_message"] = exception.what();
                    responseCode = 400;
                } catch (std::logic_error& exception) {
                    Logger::SQL_Exception(exception.what());
                    resultJson["locks"] = nlohmann::json{};
                    resultJson["status"] = "bad";
                    resultJson["error_message"] = exception.what();
                    responseCode = 400;
                }
                                
                return crow::response(responseCode, resultJson.dump());
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

        app.port(System::getEnvironmentVariables()["tripledutch"]["system"]["port"]).server_name(System::getEnvironmentVariables()["tripledutch"]["system"]["server_name"]);

        app.run();
    }
}