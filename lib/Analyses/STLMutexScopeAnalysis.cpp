
#include "cxx-langstat/Analyses/STLMutexScopeAnalysis.h"
#include "cxx-langstat/Utils.h"

// FIXME:
// Use nontrivial destructor if necessary
STLMutexScopeAnalysis::STLMutexScopeAnalysis() : ScopeAnalysis(
    clang::ast_matchers::hasAnyName("std::lock_guard", "std::unique_lock"), 
    "mutex|bits/std_mutex.h"
) {}

//-----------------------------------------------------------------------------

