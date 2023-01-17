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

    std::optional<Lock> LockManager::createNewLock(std::string _apiKey, std::string _lockName, const double LIFETIME, TDA::QueryBuilder& _queryBuilder) 
    {
        LockManager::storageMutex.lock();

        nlohmann::json results = _queryBuilder.select().from("all_locks").where("api_key = ?", {_apiKey}).where("lock_name = ?", {_lockName}).fetchAll();

        std::optional<Lock> _lock;
        if (results.empty()) {
            _lock = Lock(_apiKey, _lockName, LIFETIME);
            LockManager::allLocks.push_back(_lock.value());
        }
        LockManager::storageMutex.unlock();
        return _lock;
    }

    void LockManager::checkLifetimes()
    {
        TDA::QueryBuilder qb;

        for(;;)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            time_t now = time(0);
            struct tm* time_info = localtime(&now);
            char buffer[26];
            strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", time_info);
            std::string currentTimeStamp(buffer);

            qb.Delete().from("all_locks").where("valid_untill < ?", {currentTimeStamp}).execute();
        }
    }
}