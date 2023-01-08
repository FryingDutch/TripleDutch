#pragma once
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <optional>
#include "../headers/Lock.h"

namespace TDA
{
	class Server
	{
	private:
		static std::mutex storageMutex;
		static std::vector<Lock> lockVector;
		static std::vector<std::string> apiKeys;

	public:
		static void startup();

	public:
		Server();
		static std::optional<Lock> getLock(std::string _apiKey, std::string _lockName, const double LIFETIME);
		static std::optional<Lock> handleRequest(std::string _apiKey, std::string _lockName, const uint32_t TIMEOUT, const double LIFETIME);
		static void checkLifetimes();
		static void updateKeys();
		static void init();
	};
}