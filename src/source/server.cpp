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
#include <sstream>
#include "../headers/System.h"
#include "../headers/Lock.h"
#include "../headers/QueryBuilder.h"
#include "../headers/Logger.h"
#include "../headers/Server.h"
#include "../headers/LockManager.h"

namespace TDA
{
    std::vector<std::string> Server::apiKeys;

    std::optional<Lock> Server::handleRequest(std::string _apiKey, std::string _lockName, const uint32_t TIMEOUT, const double LIFETIME)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> difference;
        std::optional<Lock> lock;
        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;

        for (;;)
        {
            lock = LockManager::createNewLock(_apiKey, _lockName, LIFETIME);
            currentTime = std::chrono::high_resolution_clock::now();
            difference = currentTime - startTime;

            if (lock || difference.count() > TIMEOUT){
                return lock;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Server::init()
    {

        //startup();
    }

    void Server::startup()
    {
        crow::SimpleApp app;

        Logger::General_Exception("Startup (part-2) start");
        CROW_ROUTE(app, "/api/status").methods("GET"_method, "POST"_method)
            ([&](const crow::request& req) 
            {
                nlohmann::json resultJson;
                resultJson["status"] = "unknown";
                uint32_t responseCode = 200;

                if (req.url_params.get("auth") == nullptr)
                {
                    resultJson["status"] = "no api key";
                    responseCode = 400;
                    return crow::response(responseCode, resultJson.dump());
                }

                std::string userApiKey = req.url_params.get("auth");
                bool ApiKeyIsValid = false;

                for(size_t i = 0; i < Server::apiKeys.size(); i++)
                {
                    if(Server::apiKeys[i] == userApiKey)
                    {
                        ApiKeyIsValid = true;
                        break;
                    }
                }

                if(!ApiKeyIsValid)
                {
                    resultJson["status"] = "invalid API key";
                    responseCode = 403;
                    return crow::response(responseCode, resultJson.dump());
                }

                resultJson["servername"] = System::getEnvironmentVariables()["tripledutch"]["system"]["server_name"];
                resultJson["locks"] = nlohmann::json{};

                try{
                    std::unique_ptr<TDA::QueryBuilder> p_queryBuilder = std::make_unique<TDA::QueryBuilder>();
                    static std::vector<std::string>columns{"api_key", "lock_name", "session_token"};
                    static std::string table = "all_locks";
                    static std::string whereStatement = "api_key = ?";

                    try{
                        resultJson["locks"] = p_queryBuilder->select(columns)->from(table)->where(whereStatement, {userApiKey})->fetchAll();
                        resultJson["status"] = "ok";
                        resultJson["error"] = "";
                    } catch (...) {
                        Logger::General_Exception("Bad memory allocation");
                        resultJson["status"] = "ok";
                        resultJson["error"] = "bad memory allocation";
                    }

                    responseCode = 200;
                } catch (sql::SQLException& exception) {
                    Logger::SQL_Exception(exception.what(), exception.getErrorCode());
                    resultJson["locks"] = nlohmann::json{};
                    resultJson["status"] = "bad";
                    resultJson["error"] = exception.what();
                    responseCode = 400;
                } catch (std::logic_error& exception) {
                    Logger::General_Exception(exception.what());
                    resultJson["locks"] = nlohmann::json{};
                    resultJson["status"] = "bad";
                    resultJson["error"] = exception.what();
                    responseCode = 400;
                }
                                
                return crow::response(responseCode, resultJson.dump());
        });

        CROW_ROUTE(app, "/api/getlock").methods("POST"_method)
            ([&](const crow::request& req)
            {

                nlohmann::json resultJson;
                resultJson["status"] = "";
                resultJson["servername"] = System::getEnvironmentVariables()["tripledutch"]["system"]["server_name"];
                uint32_t responseCode = 200;           

                if (req.url_params.get("auth") == nullptr)
                {
                    resultJson["status"] = "no api key";
                    responseCode = 400;
                    return crow::response(responseCode, resultJson.dump());
                }
                std::string userApiKey = req.url_params.get("auth");

                bool ApiKeyIsValid = false;
                if(userApiKey.size() < 26)
                {
                    for(size_t i = 0; i < Server::apiKeys.size(); i++)
                    {
                        if(Server::apiKeys[i] == userApiKey)
                        {
                            ApiKeyIsValid = true;
                            break;
                        }
                    }
                }

                if(!ApiKeyIsValid)
                {
                    resultJson["status"] = "invalid API key";
                    responseCode = 403;
                    return crow::response(responseCode, resultJson.dump());
                }

                if (req.url_params.get("lockname") == nullptr){
                    resultJson["error"] = "no lockname supplied";
                    responseCode = 400;
                    return crow::response(responseCode, resultJson.dump());
                }
                std::string lockName = req.url_params.get("lockname");

                double lifetime;
                static int monthInSeconds = 2678400;
                if (req.url_params.get("lifetime") != nullptr && System::isDigit(req.url_params.get("lifetime")))
                {
                    lifetime = boost::lexical_cast<double>(req.url_params.get("lifetime"));
                    if(lifetime > monthInSeconds)
                    {
                        lifetime = monthInSeconds;
                    }
                } 
                
                else 
                {
                    lifetime = 30.0f;
                }

                double timeout;
                if (req.url_params.get("timeout") != nullptr && System::isDigit(req.url_params.get("timeout"))){
                    resultJson["timeout"] = req.url_params.get("timeout");
                    timeout = boost::lexical_cast<double>(req.url_params.get("timeout"));
                } else {
                    timeout = 0.0f;
                }

                if(lockName.size() < 26){                
                    std::optional<Lock> _lock = Server::handleRequest(userApiKey, lockName, timeout, lifetime);
                    resultJson["sessiontoken"] = _lock ? _lock.value().getSessionToken() : "";
                    resultJson["lockacquired"] = _lock ? true : false;
                    resultJson["lockname"] = _lock ? _lock.value().getName() : "";
                } else {
                    resultJson["status"] = "Lock name is to long";
                    responseCode = 400;
                    return crow::response(responseCode, resultJson.dump());
                }
                
                resultJson["status"] = "ok";
                responseCode = 200;

                return crow::response(responseCode, resultJson.dump());
            });

        // Releasing the lock
        CROW_ROUTE(app, "/api/releaselock").methods("DELETE"_method)
            ([&](const crow::request& req) {     
            nlohmann::json resultJson;
            uint32_t responseCode = 0;

            resultJson["status"] = "ok";
            if (req.url_params.get("auth") == nullptr || req.url_params.get("token") == nullptr){
                resultJson["status"] = "invalid API key";
                responseCode = 400;
                return crow::response(responseCode, resultJson.dump());
            }

            std::string userApiKey{req.url_params.get("auth")};
            bool ApiKeyIsValid = false;
            for(size_t i = 0; i < Server::apiKeys.size(); i++)
            {
                if(Server::apiKeys[i] == userApiKey)
                {
                    ApiKeyIsValid = true;
                    break;
                }
            }

            if(!ApiKeyIsValid)
            {
                resultJson["status"] = "invalid API key";
                responseCode = 403;
                return crow::response(responseCode, resultJson.dump());
            }

            if (req.url_params.get("lockname") == nullptr){
                resultJson["error"] = "no lockname supplied";                
                responseCode = 400;
                return crow::response(responseCode, resultJson.dump());
            }

            std::string session_token{req.url_params.get("token")};
            std::string lockName{req.url_params.get("lockname")};

            std::unique_ptr<TDA::QueryBuilder> p_queryBuilder = std::make_unique<TDA::QueryBuilder>();
            p_queryBuilder->Delete()->from("all_locks")->where("lock_name = ?", {lockName})->And("api_key = ?", {userApiKey})->And("session_token = ?", {session_token})->execute();

            return crow::response(200, resultJson.dump());
        });

        std::thread _lifeTime_thread(&LockManager::checkLifetimes);
        std::thread _apiKey_update_thread(&Server::updateKeys);
        std::thread _remove_old_locks_thread(&LockManager::removeExpiredLocks);

        try{
            app.port(System::getEnvironmentVariables()["tripledutch"]["system"]["port"]).server_name(System::getEnvironmentVariables()["tripledutch"]["system"]["server_name"]).ssl_file(System::getEnvironmentVariables()["tripledutch"]["system"]["ssl_crt"], System::getEnvironmentVariables()["tripledutch"]["system"]["ssl_key"]).concurrency(100);
        } catch (boost::wrapexcept<boost::system::system_error>& ex) {
            std::cerr << ex.what();
            Logger::General_Exception(ex.what());
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            Logger::General_Exception(ex.what());
        } catch (...) {
            Logger::General_Exception("Unknown error during crow configuration");
        }

        app.run();

        _lifeTime_thread.join();
        _apiKey_update_thread.join();
        _remove_old_locks_thread.join();
    }

    void Server::updateKeys()
    {
        std::unique_ptr<TDA::QueryBuilder> p_queryBuilder = std::make_unique<TDA::QueryBuilder>();
        std::vector<std::vector<std::string>> query_result;
        std::vector<std::string> all_keys;

        for(;;)
        {
            query_result = p_queryBuilder->select({"value"})->from("api_keys")->fetchAll();
            all_keys = {};
            
            for(size_t i = 0; i < query_result.size(); i++)
            {
                all_keys.push_back(query_result[i][0]);
            }

            Server::apiKeys = all_keys;
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
    }
}