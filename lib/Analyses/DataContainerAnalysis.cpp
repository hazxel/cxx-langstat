
#include "cxx-langstat/Analyses/DataContainerAnalysis.h"
#include "cxx-langstat/Utils.h"

// FIXME:
// Use nontrivial destructor if necessary
DataContainerAnalysis::DataContainerAnalysis() : ClassMethodCallAnalysis(
    clang::ast_matchers::hasAnyName(
        // Standard library containers, Copied from UseAutoCheck.cpp
        "std::array", "std::vector", "std::deque",
        "std::forward_list", "std::list",
        "std::set", "std::map",
        "std::multiset", "std::multimap",
        "std::unordered_set", "std::unordered_map",
        "std::unordered_multiset", "std::unordered_multimap",
        "std::stack", "std::queue", "std::priority_queue"
        // TBB
        "tbb::concurrent_vector", "tbb::concurrent_unordered_map", "tbb::concurrent_unordered_set", "tbb::concurrent_queue",
        // PPL
        "concurrency::concurrent_vector", "concurrency::concurrent_queue", "concurrency::concurrent_stack",
        "concurrency::concurrent_unordered_map", "concurrency::concurrent_unordered_multimap"
    ),
    // libc++
    "vector|list|forward_list|array|map|set|unordered_map|unordered_set|queue|deque|priority_queue|stack|"
    // tbb
    "tbb/concurrent_vector.h|tbb/concurrent_unordered_map.h|tbb/concurrent_unordered_set.h|tbb/concurrent_queue.h|"
    // ppl
    "concurrent_vector.h|concurrent_queue.h|concurrent_stack.h|concurrent_unordered_map.h|concurrent_unordered_multimap.h"
) {}