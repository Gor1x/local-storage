#ifndef LOCAL_STORAGE_PERSISTENTHASHTABLE_H
#define LOCAL_STORAGE_PERSISTENTHASHTABLE_H

#include <string>
#include <fstream>
#include <unordered_map>
#include <ios>
#include <log.h>
#include <thread>
#include <atomic>


class PersistentHashTable {
public:
    typedef std::string HTKey;
    typedef uint64_t HTValue;
    typedef std::unordered_map<HTKey, HTValue> HashTable;
    typedef HashTable::iterator iterator;

    explicit PersistentHashTable(const std::string &filename);

    ~PersistentHashTable();

    HTValue &operator[](const HTKey &key);

    iterator begin();

    iterator find(const HTKey &key);

    iterator end();

    void clear();

    void erase(const HTKey &key);

    bool contains(const HTKey &key);
private:
    mutable std::mutex mutex;
    std::thread syncThread;
    HashTable storage;
    const int FLUSH_THRESHOLD = 1000;
    std::atomic_bool needSync;

    void getDataFromSource(const std::string &filename);
};

#endif //LOCAL_STORAGE_PERSISTENTHASHTABLE_H
