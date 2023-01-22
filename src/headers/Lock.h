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
        Lock(uint32_t _id, std::string _apiKey, std::string _name, std::string _session_token, double _lifeTime);
        //~Lock();

        bool expired();
        double timeLeft();
        std::string getApiKey();
        std::string getName();
        std::string getSessionToken();
        double getLifeTime();

        void setId(uint32_t _id) {m_id = _id;}
        void setApiKey(std::string _apiKey) {m_apiKey = _apiKey;}
        void setName(std::string _name) {m_name = _name;}
        void setSessionToken(std::string _session_token) {m_session_token = _session_token;}
        void setLifeTime(double _lifeTime) {m_lifeTime = _lifeTime;}
    };
}