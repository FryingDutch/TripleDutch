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

namespace TDA
{
    //runtime functions
    void Server::startup()
    {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/status")
            ([&](const crow::request& req) {

            crow::json::wvalue x;
            x["servername"] = "Helo world!";
            return crow::response(200, x);
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