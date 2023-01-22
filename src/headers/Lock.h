#pragma once
#include <string>
#include <chrono>
#include "../headers/QueryBuilder.h"

namespace TDA
{
    class Lock
    {
    private:
        uint32_t m_id;
        std::string m_apiKey;
        std::string m_name;
        std::string m_session_token;
        double m_lifeTime;
        std::chrono::_V2::system_clock::time_point m_start;
        
    private:
        std::string createToken();

    public:
        Lock(std::string _apiKey, std::string _name, double _lifeTime);
        Lock(uint32_t _id, std::string _apiKey, std::string _name, std::string _session_token, double _lifeTime);
        //~Lock();

        bool expired();
        double timeLeft();
        std::string getApiKey();
        std::string getName();
        std::string getSessionToken();
        double getLifeTime();

        void removeFromDatabase();
    };
}