#include <string>
#include <unordered_set>


class Deduplicator {
public:
    bool isDuplicated(std::string key);

private:
    std::unordered_set<std::string> duplicates_;
};