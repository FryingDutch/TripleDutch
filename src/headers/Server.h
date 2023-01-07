#pragma once
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace TDA
{
	class Server
	{
	private:

	public:
		static void startup();

	public:
		Server();
		nlohmann::json getEnvironmentVariables();
	};
}