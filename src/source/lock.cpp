#include <random>
#include <algorithm>
#include "../headers/Lock.h"

namespace TDA
{
	Lock::Lock(std::string _apiKey, std::string _name, double _lifeTime)
		: m_apiKey(_apiKey), m_name(_name), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
		m_session_token = Lock::createToken();
	}

	Lock::Lock(uint32_t _id, std::string _apiKey, std::string _name, std::string _session_token, double _lifeTime)
		: m_id(_id), m_apiKey(_apiKey), m_name(_name), m_session_token(_session_token), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	std::string Lock::createToken()
	{				
		static std::string str =
			"01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		static std::random_device rd;
		static std::mt19937 generator(rd());

		std::shuffle(str.begin(), str.end(), generator);
		std::string newToken = str.substr(0, 32);

		return newToken.substr(0, 32);
	}

	bool Lock::expired()
	{
		return this->timeLeft() < 0;
	}

	double Lock::timeLeft() 
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> difference = currentTime - m_start;

		return this->m_lifeTime - difference.count();
	}
    
    std::string Lock::getApiKey() { return m_apiKey; }
	std::string Lock::getName() { return m_name; }
	std::string Lock::getSessionToken() { return m_session_token; }
	double Lock::getLifeTime() { return m_lifeTime; }
}