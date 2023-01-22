#pragma once
#include <string>
#include <mutex>
#include <optional>
#include "../headers/QueryBuilder.h"

namespace TDA
{
    class LockManager
    {
    public:
		static std::vector<Lock> allLocks;
        static std::mutex storageMutex;

    public:
        static std::optional<Lock> createNewLock(std::string _apiKey, std::string _lockName, const double _LIFETIME, TDA::QueryBuilder*& _queryBuilder);
        static void checkLifetimes();
        static void removeExpiredLocks();
    };
}