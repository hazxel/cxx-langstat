
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
        "std::promise", "std::future", "std::shared_future",// futures
        // pthreads
        "pthread_t", "pthread_mutex_t", "pthread_cond_t", "pthread_rwlock_t",
        "pthread_create", "pthread_join", "pthread_detach", "pthread_exit", "pthread_cancel",
        // openmp
        "omp_lock_t", "omp_nest_lock_t", "omp_sched_t", "omp_sched_t", "omp_proc_bind_t",
        "omp_init_lock", "omp_init_nest_lock", "omp_destroy_lock", "omp_destroy_nest_lock",
        "omp_set_lock", "omp_set_nest_lock", "omp_unset_lock", "omp_unset_nest_lock",
        "omp_get_thread_num", "omp_get_num_threads", "omp_get_max_threads", "omp_get_num_procs"
    ),
    // libc++:
    "thread|atomic|mutex|conditional_variable|future|"
    // libstdc++
    "bits/std_thread.h|std/atomic|bits/std_mutex.h|std/condition_variable|std/future|"
    // pthreads
    "pthread.h|"
    // openmp
    "omp.h"
) {}

//-----------------------------------------------------------------------------

// void ConcurrencySupportLibraryMethodAnalysis::processFeatures(nlohmann::ordered_json j){
//     if(j.contains(method_call_key_)){
//         ordered_json res;
//         templatePrevalence(j.at(method_call_key_), res);
//         Statistics[method_call_key_] = res;
//     }
// }

//-----------------------------------------------------------------------------

