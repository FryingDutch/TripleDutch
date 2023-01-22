#include "../headers/Lock.h"
#include "../headers/LockManager.h"
#include "../headers/QueryBuilder.h"
#include <optional>
#include <nlohmann/json.hpp>
#include <thread>

namespace TDA
{
    std::vector<Lock> LockManager::allLocks;
    std::mutex LockManager::storageMutex;

    std::optional<Lock> LockManager::createNewLock(std::string _apiKey, std::string _lockName, const double _LIFETIME, TDA::QueryBuilder*& _queryBuilder) 
    {
        LockManager::storageMutex.lock();

        nlohmann::json results = _queryBuilder->select()->from("all_locks")->where("api_key = ?", {_apiKey})->where("lock_name = ?", {_lockName})->fetchAll();

        std::optional<Lock> lock;
        if (results.empty()) {
            lock = Lock(_apiKey, _lockName, _LIFETIME);
            LockManager::allLocks.push_back(lock.value());
        }
        LockManager::storageMutex.unlock();
        return lock;
    }

    void LockManager::checkLifetimes()
    {
        for(;;)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            LockManager::storageMutex.lock();
            for (size_t i = 0; i < LockManager::allLocks.size(); i++)
            {
                if (LockManager::allLocks[i].expired())
                {
                    LockManager::allLocks[i].removeFromDatabase();
                    LockManager::allLocks.erase(LockManager::allLocks.begin() + i);
                    LockManager::allLocks.shrink_to_fit();
                }
            }
            LockManager::storageMutex.unlock();
        }
    }

    void LockManager::removeExpiredLocks()
    {
        std::unique_ptr<TDA::QueryBuilder> p_queryBuilder = std::make_unique<TDA::QueryBuilder>();

        for(;;)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            time_t now = time(0);
            struct tm* time_info = localtime(&now);
            char buffer[26];
            strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", time_info);
            std::string currentTimeStamp(buffer);

            p_queryBuilder->Delete()->from("all_locks")->where("valid_untill < ?", {currentTimeStamp})->execute();
        }
    }
}