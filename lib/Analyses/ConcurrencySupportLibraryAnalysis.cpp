#include <iostream>

#include "cxx-langstat/Analyses/ConcurrencySupportLibraryAnalysis.h"
#include "cxx-langstat/Utils.h"

using ordered_json = nlohmann::ordered_json;
template<typename T>
using StringMap = std::map<std::string, T>;

//-----------------------------------------------------------------------------

// Construct a CSLA by constructing a more constrained TIA.
ConcurrencySupportLibraryAnalysis::ConcurrencySupportLibraryAnalysis() : ClassInstantiationAnalysis (
    clang::ast_matchers::hasAnyName(
        // standard library concurrency supports
        "std::thread", "std::jthread", // threads
        "std::atomic", // atomic operations
        "std::mutex", "std::lock_guard", "std::unique_lock", // locking
        "std::condition_variable", // condition variables
        "std::future",
        // pthreads
        "pthread_t", "pthread_mutex_t", "pthread_cond_t", "pthread_rwlock_t"
    ),
    // libc++:
    "thread|atomic|mutex|conditional_variable|future|"
    // libstdc++
    "bits/std_thread.h|std/atomic|bits/std_mutex.h|std/condition_variable|std/future") {
    std::cout << "CSLA ctor\n";
}

namespace {
    const StringMap<int> NumRelTypes = {
        // no constexpr support for map, also, can't use non-compile-time
        // expressions in inline initialization of static data member
        {"std::cout", 0},
        // standard library concurrency supports
        {"std::thread", 0}, {"std::jthread", 0}, {"std::atomic", 1},
        {"std::mutex", 0}, {"std::lock_guard", 1}, {"std::unique_lock", 1},
        {"std::condition_variable", 0}, {"std::future", 1}
        // pthread
        // {"pthread_t", 0}, {"pthread_mutex_t", 0}, {"pthread_cond_t", 0},
        // {"pthread_rwlock_t", 0}
    };
} // namespace

// Gathers data on how often each utility template was used.
void concurrencyPrevalence(const ordered_json& in, ordered_json& out){
    templatePrevalence(in, out);
}

// For each container template, gives statistics on how often each instantiation
// was used by a (member) variable.
void concurrencyTemplateTypeArgPrevalence(const ordered_json& in, ordered_json& out){
    templateTypeArgPrevalence(in, out, NumRelTypes);
}

void ConcurrencySupportLibraryAnalysis::processFeatures(nlohmann::ordered_json j) {
    if(j.contains(VarKey)){
        ordered_json res;
        templatePrevalence(j.at(VarKey), res);
        Statistics[VarKey] = res;
    }
}

//-----------------------------------------------------------------------------

