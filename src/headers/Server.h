#pragma once
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <optional>
#include "../headers/Lock.h"
#include "../headers/QueryBuilder.h"

namespace TDA
{
	class Server
	{
	private:
		static std::vector<std::string> apiKeys;

	private:
		static void startup();

	public:
		Server();
		static std::optional<Lock> handleRequest(std::string _apiKey, std::string _lockName, const uint32_t TIMEOUT, const double LIFETIME);
		static void updateKeys();
		static void init();
	};
}