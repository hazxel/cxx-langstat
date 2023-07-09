
#ifndef STLMUTEXSCOPEANALYSIS_H
#define STLMUTEXSCOPEANALYSIS_H

#include "cxx-langstat/Analyses/ScopeAnalysis.h"

//-----------------------------------------------------------------------------

class STLMutexScopeAnalysis : public ScopeAnalysis {
public:
    STLMutexScopeAnalysis();
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    static constexpr auto ShorthandName = "smsa";
};

//-----------------------------------------------------------------------------

#endif // STLMUTEXSCOPEANALYSIS_H
