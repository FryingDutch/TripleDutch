#pragma once
#include <string>
#include <chrono>
#include <mutex>

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
        bool expired();
        double timeLeft();
        std::string getApiKey();
        std::string getName();
        std::string getSessionToken();
    };
}