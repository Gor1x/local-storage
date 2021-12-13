#include <mutex>
#include "../inc/PersistentHashTable.h"


using namespace NLogging;

namespace {
    typedef std::string HTKey;
    typedef uint64_t HTValue;
    typedef std::unordered_map<HTKey, HTValue> HashTable;
    typedef HashTable::iterator iterator;
} // namespace


PersistentHashTable::PersistentHashTable(const std::string &filename) {
    {
        LOG_INFO_S("Read storage data from " << filename);
    }
    getDataFromSource(filename);
    needSync = true;
    syncThread = std::thread([&, filename]() {
        while (needSync) {
            std::lock_guard<std::mutex> g(mutex);
            std::ofstream file(filename, std::ios_base::out);
            for (auto &x: storage) {
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
    return storage.begin();
}

iterator PersistentHashTable::end() {
    return storage.end();
}

iterator PersistentHashTable::find(const HTKey &key) {
    return storage.find(key);
}

HTValue &PersistentHashTable::operator[](const HTKey &key) {
    return storage[key];
}

bool PersistentHashTable::contains(const HTKey &key) {
    return storage.count(key) == 1;
}

void PersistentHashTable::erase(const HTKey &key) {
    storage.erase(key);
}

void PersistentHashTable::clear() {
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
            LOG_INFO_S( "Key " << key << " Value " << value);
        }
    }
    inputFile.close();
}
