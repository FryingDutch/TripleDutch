#include <random>
#include <algorithm>
#include "../headers/Lock.h"
#include "../headers/QueryBuilder.h"

namespace TDA
{
	// Constructor used to create a new lock
	Lock::Lock(std::string _apiKey, std::string _name, double _lifeTime)
		: m_apiKey(_apiKey), m_name(_name), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
		m_session_token = Lock::createToken();

		time_t totalLifeTimeInSecondsSince1970 = time(0) + _lifeTime;
        struct tm* time_info = localtime(&totalLifeTimeInSecondsSince1970);
        char buffer[26];

        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", time_info);
        std::string lifeTime_TimeStamp(buffer);

		std::unique_ptr<TDA::QueryBuilder> p_queryBuilder = std::make_unique<TDA::QueryBuilder>();
        std::vector<std::string>columns{"api_key", "lock_name", "session_token", "valid_untill"};
        p_queryBuilder->insert("all_locks", columns, {m_apiKey, m_name, m_session_token, lifeTime_TimeStamp})->execute();
	}

	// Constructor used for mapping all locks from the database at startup
	Lock::Lock(long unsigned int _id, std::string _apiKey, std::string _name, std::string _session_token, double _lifeTime)
		: m_id(_id), m_apiKey(_apiKey), m_name(_name), m_session_token(_session_token), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	// Create unique token for each lock, which is needed to release the lock on request
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

	// Check whether or not the lock is expired
	bool Lock::expired()
	{
		return this->timeLeft() < 0;
	}

	// Return the time left of the total life time of a lock
	double Lock::timeLeft() 
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> difference = currentTime - m_start;

		return this->m_lifeTime - difference.count();
	}

	// Used as a temp function to remove entry from database on deletion.
	// TO-DO: Need to make a deconstructor with an instance counter.
	void Lock::removeFromDatabase()
	{
		std::unique_ptr<TDA::QueryBuilder> ptr_queryBuilder = std::make_unique<TDA::QueryBuilder>();
		ptr_queryBuilder->Delete()->from("all_locks")->where("lock_name = ?", {m_name})->And("api_key = ?", {m_apiKey})->And("session_token = ?", {m_session_token})->execute();
	}
    
	// Getter functions
    std::string Lock::getApiKey() { return m_apiKey; }
	std::string Lock::getName() { return m_name; }
	std::string Lock::getSessionToken() { return m_session_token; }
	double Lock::getLifeTime() { return m_lifeTime; }
}