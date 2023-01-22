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
        TDA::QueryBuilder* p_queryBuilder = new TDA::QueryBuilder;
        auto startTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> difference;
        std::optional<Lock> lock;
        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;

        for (;;)
        {
            lock = LockManager::createNewLock(_apiKey, _lockName, LIFETIME, p_queryBuilder);
            currentTime = std::chrono::high_resolution_clock::now();
            difference = currentTime - startTime;

            if (lock || difference.count() > TIMEOUT){
                delete p_queryBuilder;
                return lock;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Server::init()
    {
        std::unique_ptr<TDA::QueryBuilder> p_queryBuilder = std::make_unique<TDA::QueryBuilder>();
        std::vector<std::vector<std::string>> allLocks = p_queryBuilder->select()->from("all_locks")->fetchAll();

        enum {
            ID = 0, API_KEY, LOCK_NAME, SESSION_TOKEN, VALID_UNTILL
        };

        for(size_t i = 0; i < allLocks.size(); i++)
        {  
            std::istringstream ss(allLocks[i][VALID_UNTILL]);
            std::tm tm_timeStamp = {};
            ss >> std::get_time(&tm_timeStamp, "%Y-%m-%d %H:%M:%S");
            time_t validUntill = mktime(&tm_timeStamp);

            time_t now = time(0);
            double difference = validUntill - now;
            TDA::Lock lock{std::stoul(allLocks[i][ID]), allLocks[i][API_KEY], allLocks[i][LOCK_NAME], allLocks[i][SESSION_TOKEN], difference};
            
            TDA::LockManager::allLocks.push_back(lock);
        }

        startup();
    }

    void Server::startup()
    {
        crow::SimpleApp app;

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

                    resultJson["locks"] = p_queryBuilder->select(columns)->from(table)->where(whereStatement, {userApiKey})->fetchAll();
                    resultJson["status"] = "ok";
                    resultJson["error"] = "";
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

                std::vector<std::string>values;             

                if (req.url_params.get("auth") == nullptr){
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

                values.push_back(req.url_params.get("auth"));

                if (req.url_params.get("lockname") == nullptr){
                    resultJson["error"] = "no lockname supplied";
                    responseCode = 400;
                    return crow::response(responseCode, resultJson.dump());
                }
                std::string lockName = req.url_params.get("lockname");
                values.push_back(lockName);

                double lifetime;
                if (req.url_params.get("lifetime") != nullptr && System::isDigit(req.url_params.get("lifetime"))){
                    resultJson["lifetime"] = req.url_params.get("lifetime");
                    lifetime = boost::lexical_cast<double>(req.url_params.get("lifetime"));
                } else {
                    lifetime = 30.0f;
                }

                double timeout;
                if (req.url_params.get("timeout") != nullptr && System::isDigit(req.url_params.get("timeout"))){
                    resultJson["timeout"] = req.url_params.get("timeout");
                    timeout = boost::lexical_cast<double>(req.url_params.get("timeout"));
                }

                else {
                    timeout = 0.0f;
                }

                std::optional<Lock> _lock = Server::handleRequest(userApiKey, lockName, timeout, lifetime);

                resultJson["sessiontoken"] = _lock ? _lock.value().getSessionToken() : "";
                resultJson["lockacquired"] = _lock ? true : false;
                resultJson["lockname"] = _lock ? _lock.value().getName() : "";
                
                values.push_back(resultJson["sessiontoken"]);

                time_t now = time(0) + lifetime;
                struct tm* time_info = localtime(&now);
                char buffer[26];

                strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", time_info);
                std::string validUntill(buffer);
                values.push_back(validUntill);

                std::vector<std::string>columns{"api_key", "lock_name", "session_token", "valid_untill"};

                try{
                    if(resultJson["lockacquired"]){
                        std::unique_ptr<TDA::QueryBuilder> p_queryBuilder = std::make_unique<TDA::QueryBuilder>();
                        std::string apiTable{"api_keys"};
                        p_queryBuilder->insert("all_locks", columns, values)->execute();
                    }
                    resultJson["status"] = "ok";
                    responseCode = 200;
                } catch (sql::SQLException& exception) {
                    Logger::SQL_Exception(exception.what());
                    resultJson["status"] = "bad";
                    resultJson["error"] = exception.what();
                    responseCode = 400;
                } catch (std::logic_error& exception) {
                    Logger::SQL_Exception(exception.what());
                    resultJson["status"] = "bad";
                    resultJson["error"] = exception.what();
                    responseCode = 400;
                } catch (...) {
                    Logger::SQL_Exception("Unknown Error");
                    resultJson["status"] = "bad";
                    resultJson["error"] = "Unknown Error";
                    responseCode = 400;
                }

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