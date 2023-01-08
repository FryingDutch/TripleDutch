#include <random>
#include <algorithm>
#include "../headers/Lock.h"

namespace TDA
{
	Lock::Lock(std::string _apiKey, std::string _name, double _lifeTime)
		: m_apiKey(_apiKey), m_name(_name), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
		m_session_token = Lock::m_createToken();
	}

	std::string Lock::m_createToken()
	{				
		static std::string str =
			"01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		static std::random_device rd;
		static std::mt19937 generator(rd());

		std::shuffle(str.begin(), str.end(), generator);
		std::string newToken = str.substr(0, 32);

		return newToken.substr(0, 32);
	}

	bool Lock::m_expired()
	{
		return this->m_timeLeft() < 0;
	}

	double Lock::m_timeLeft() 
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> difference = currentTime - m_start;

		return this->m_lifeTime - difference.count();
	}
    
    std::string Lock::getApiKey() { return this->m_apiKey; }
	std::string Lock::getName() { return this->m_name; }
	std::string Lock::m_getSessionToken() { return this->m_session_token; }
}