
#include "cxx-langstat/Analyses/STLMutexScopeAnalysis.h"
#include "cxx-langstat/Utils.h"

// FIXME:
// Use nontrivial destructor if necessary
STLMutexScopeAnalysis::STLMutexScopeAnalysis() : ScopeAnalysis(
    clang::ast_matchers::hasAnyName(
        "std::lock_guard", "std::unique_lock", "std::scoped_lock", "std::shared_lock",
        "boost::lock_guard", "boost::unique_lock", "boost::scoped_lock", "boost::shared_lock", 
        "tbb::mutex::scoped_lock"
    ), 
    "mutex|shared_mutex|bits/std_mutex.h|std/shared_mutex|"
    "boost/thread/lock_guard.hpp|boost/thread/shared_lock_guard.hpp|boost/thread/mutex.hpp|boost/sync/locks/shared_lock.hpp|"
    "tbb/mutex.h"
) {}

//-----------------------------------------------------------------------------

