#include <iostream>

#include "cxx-langstat/Analyses/ConcurrencySupportLibraryAnalysis.h"
#include "cxx-langstat/Utils.h"

using ordered_json = nlohmann::ordered_json;

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
    // std::cout << "CSLA ctor\n";
}


// Gathers data on how often each utility template was used.
void concurrencyPrevalence(const ordered_json& in, ordered_json& out){
    templatePrevalence(in, out);
}


void ConcurrencySupportLibraryAnalysis::processFeatures(nlohmann::ordered_json j) {
    if(j.contains(VarKey)){
        ordered_json res;
        concurrencyPrevalence(j.at(VarKey), res);
        Statistics[VarKey] = res;
    }
}

//-----------------------------------------------------------------------------

