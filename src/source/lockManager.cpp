#include "../headers/Lock.h"
#include "../headers/LockManager.h"
#include "../headers/QueryBuilder.h"
#include <optional>
#include <nlohmann/json.hpp>

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
}