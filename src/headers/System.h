#pragma once
#include <mutex>
#include <string>
#include <nlohmann/json.hpp>

namespace TDA
{
    class System
    {
	private:
		static std::mutex storageMutex;
        static nlohmann::json environmentVariables;

    public:
        //functions used to define the settings of the server
        static void init();
		static void errormsg(const char* message);
		static bool isDigit(std::string str);
        static nlohmann::json getEnvironmentVariables(std::vector<std::string>_jsonKeys = {});
    };
}