#ifndef DEDUPLICATOR_H
#define DEDUPLICATOR_H

#include <string>
#include <unordered_set>


class Deduplicator {
public:
    bool isDuplicated(const std::string& key);

private:
    std::unordered_set<std::string> duplicates_;
};

#endif // DEDUPLICATOR_H