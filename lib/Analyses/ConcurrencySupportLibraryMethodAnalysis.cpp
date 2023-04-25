
#include "cxx-langstat/Analyses/ConcurrencySupportLibraryMethodAnalysis.h"
#include "cxx-langstat/Utils.h"

using ordered_json = nlohmann::ordered_json;
// FIXME:
// Use nontrivial destructor if necessary
ConcurrencySupportLibraryMethodAnalysis::ConcurrencySupportLibraryMethodAnalysis() : ClassMethodCallAnalysis(
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
    "bits/std_thread.h|std/atomic|bits/std_mutex.h|std/condition_variable|std/future"
) {
    std::cout << "CSLMA ctor\n";
}

//-----------------------------------------------------------------------------

// void ConcurrencySupportLibraryMethodAnalysis::processFeatures(nlohmann::ordered_json j){
//     if(j.contains(method_call_key_)){
//         ordered_json res;
//         templatePrevalence(j.at(method_call_key_), res);
//         Statistics[method_call_key_] = res;
//     }
// }

//-----------------------------------------------------------------------------

