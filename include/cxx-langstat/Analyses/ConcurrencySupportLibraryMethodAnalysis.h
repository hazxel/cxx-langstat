
#ifndef CONCURRENCYSUPPORTLIBRARYMETHODANALYSIS_H
#define CONCURRENCYSUPPORTLIBRARYMETHODANALYSIS_H

#include "cxx-langstat/Analyses/ClassMethodCallAnalysis.h"

//-----------------------------------------------------------------------------

class ConcurrencySupportLibraryMethodAnalysis : public ClassMethodCallAnalysis {
public:
    ConcurrencySupportLibraryMethodAnalysis();
    ~ConcurrencySupportLibraryMethodAnalysis(){
        // std::cout << "CSLMA dtor\n";
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    // void processFeatures(nlohmann::ordered_json j) override;
    static constexpr auto ShorthandName = "cslma";
};

//-----------------------------------------------------------------------------

#endif // CONCURRENCYSUPPORTLIBRARYMETHODANALYSIS_H
