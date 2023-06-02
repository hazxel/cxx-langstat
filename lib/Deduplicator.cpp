#include "cxx-langstat/Deduplicator.h"

bool Deduplicator::isDuplicated(std::string key) {
    if (duplicates_.find(key) != duplicates_.end()) {
        return true;
    }
    duplicates_.insert(key);
    return false;
}