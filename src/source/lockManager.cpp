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

    std::optional<Lock> LockManager::createNewLock(std::string _apiKey, std::string _lockName, const double _LIFETIME) 
    {
        LockManager::storageMutex.lock();
        bool lockIsFree{true};

        for (size_t i = 0; i < LockManager::allLocks.size(); i++)
        {
            if (_lockName == LockManager::allLocks[i].getName() && !LockManager::allLocks[i].expired())
            {
                lockIsFree = false;
                break;
            }
        }

        std::optional<Lock> lock;
        if (lockIsFree) {
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
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            time_t now = time(0);
            struct tm* time_info = localtime(&now);
            char buffer[26];
            strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", time_info);
            std::string currentTimeStamp(buffer);

            p_queryBuilder->Delete()->from("all_locks")->where("valid_untill < ?", {currentTimeStamp})->execute();
        }
    }
}