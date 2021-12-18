#include <mutex>
#include "../inc/PersistentHashTable.h"


using namespace NLogging;


PersistentHashTable::PersistentHashTable(const std::string &filename) {
    {
        LOG_INFO_S("Read storage data from " << filename);
    }
    getDataFromSource(filename);
    needSync = true;
    syncThread = std::thread([&, filename]() {
        while (needSync) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
            mutex.lock();
            auto save = storage;
            mutex.unlock();

            std::ofstream file(filename, std::ios_base::out);
            for (auto &x: save) {
                file << x.first << " " << x.second << "\n";
            }
            file.close();
        }
    });
}

PersistentHashTable::~PersistentHashTable() {
    needSync = false;
    syncThread.join();
}

iterator PersistentHashTable::begin() {
    std::lock_guard<std::mutex> g(mutex);
    return storage.begin();
}

iterator PersistentHashTable::end() {
    std::lock_guard<std::mutex> g(mutex);
    return storage.end();
}

iterator PersistentHashTable::find(const HTKey &key) {
    std::lock_guard<std::mutex> g(mutex);
    return storage.find(key);
}

HTValue &PersistentHashTable::operator[](const HTKey &key) {
    std::lock_guard<std::mutex> g(mutex);
    return storage[key];
}

bool PersistentHashTable::contains(const HTKey &key) {
    std::lock_guard<std::mutex> g(mutex);
    return storage.find(key) != end();
}

void PersistentHashTable::erase(const HTKey &key) {
    std::lock_guard<std::mutex> g(mutex);
    storage.erase(key);
}

void PersistentHashTable::clear() {
    std::lock_guard<std::mutex> g(mutex);
    storage.clear();
}

void PersistentHashTable::getDataFromSource(const std::string &filename) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        LOG_WARN("Can't open file with data")
        inputFile.close();
        return;
    }
    HTKey key;
    HTValue value;
    while (inputFile >> key >> value) {
        storage[key] = value;
        {
            LOG_DEBUG_S("Key " << key << " Value " << value);
        }
    }
    inputFile.close();
}
