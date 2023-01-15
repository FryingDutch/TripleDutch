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
#include "../headers/Lock.h"
#include "../headers/QueryBuilder.h"
#include "../headers/Logger.h"
#include "../headers/Server.h"

namespace TDA
{
    std::vector<Lock> Server::lockVector;
    std::vector<std::string> Server::apiKeys;
    std::mutex Server::storageMutex;

    std::optional<Lock> Server::getLock(std::string _apiKey, std::string _lockName, const double LIFETIME, TDA::QueryBuilder& _queryBuilder) 
    {
        Server::storageMutex.lock();

        nlohmann::json results = _queryBuilder.select().from("all_locks").where("api_key = ?", {_apiKey}).where("lock_name = ?", {_lockName}).fetchAll();
        Logger::SQL_Info(results.dump());   

        std::optional<Lock> _lock;
        if (results.empty()) {
            _lock = Lock(_apiKey, _lockName, LIFETIME);
            Server::lockVector.push_back(_lock.value());
        }
        Server::storageMutex.unlock();
        return _lock;
    }

    std::optional<Lock> Server::handleRequest(std::string _apiKey, std::string _lockName, const uint32_t TIMEOUT, const double LIFETIME)
    {
        TDA::QueryBuilder qb;
        auto startTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> difference;
        for (;;)
        {
            std::optional<Lock> lock = Server::getLock(_apiKey, _lockName, LIFETIME, qb);
            auto currentTime = std::chrono::high_resolution_clock::now();
            difference = currentTime - startTime;

            if (lock || difference.count() > TIMEOUT) 
                return lock;

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void Server::init()
    {
        TDA::QueryBuilder qb;
        qb.select().from("all_locks");
    }

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
                    static std::vector<std::string>columns{"api_key", "lock_name", "session_token"};
                    static std::string table = "all_locks";
                    static std::string whereStatement = "api_key = ?";

                    resultJson["locks"] = qb.select(columns).from(table).where(whereStatement, {userApiKey}).fetchAll();
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

        CROW_ROUTE(app, "/getlock")
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

                resultJson["sessiontoken"] = _lock ? _lock.value().m_getSessionToken() : "";
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
                        TDA::QueryBuilder qb;
                        std::string apiTable = "api_keys";
                        qb.insert("all_locks", columns, values).execute();
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
        CROW_ROUTE(app, "/releaselock")
            ([&](const crow::request& req) {

            std::string session_token, userApiKey, lockName;      
            nlohmann::json resultJson;
            uint32_t responseCode = 0;

            resultJson["status"] = "ok";
            if (req.url_params.get("auth") == nullptr || req.url_params.get("token") == nullptr){
                resultJson["status"] = "invalid API key";
                responseCode = 400;
                return crow::response(responseCode, resultJson.dump());
            }

            userApiKey = req.url_params.get("auth");

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

            userApiKey = req.url_params.get("auth");
            session_token = req.url_params.get("token");
            lockName = req.url_params.get("lockname");

            TDA::QueryBuilder qb;
            qb.Delete().from("all_locks").where("lock_name = ?", {lockName}).And("api_key = ?", {userApiKey}).And("session_token = ?", {session_token}).execute();

            return crow::response(200, resultJson.dump());
        });

        std::thread _lifeTime_thread(&Server::checkLifetimes);
        std::thread _apiKey_update_thread(&Server::updateKeys);

        Logger::General_Info(System::getEnvironmentVariables()["tripledutch"]["system"]["ssl_crt"]);
        Logger::General_Info(System::getEnvironmentVariables()["tripledutch"]["system"]["ssl_key"]);

        try{
            app.port(System::getEnvironmentVariables()["tripledutch"]["system"]["port"]).server_name(System::getEnvironmentVariables()["tripledutch"]["system"]["server_name"]).ssl_file(System::getEnvironmentVariables()["tripledutch"]["system"]["ssl_crt"], System::getEnvironmentVariables()["tripledutch"]["system"]["ssl_key"]);
        } catch (boost::wrapexcept<boost::system::system_error>& ex) {
            std::cerr << ex.what();
            Logger::SQL_Exception(ex.what());
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            Logger::SQL_Exception(ex.what());
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            Logger::SQL_Exception(ex.what());
        }

        app.run();

        _lifeTime_thread.join();
        _apiKey_update_thread.join();
    }

    void Server::checkLifetimes()
    {
        TDA::QueryBuilder qb;

        for(;;)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            time_t now = time(0);
            struct tm* time_info = localtime(&now);
            char buffer[26];
            strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", time_info);
            std::string currentTimeStamp(buffer);

            qb.Delete().from("all_locks").where("valid_untill < ?", {currentTimeStamp}).execute();
        }
    }

    void Server::updateKeys()
    {
        TDA::QueryBuilder qb;
        std::vector<std::vector<std::string>> query_result;
        std::vector<std::string> all_keys;

        for(;;)
        {
            query_result = qb.select({"value"}).from("api_keys").fetchAll();
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