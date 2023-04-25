
#ifndef CONCURRENCYSUPPORTLIBRARYANALYSIS_H
#define CONCURRENCYSUPPORTLIBRARYANALYSIS_H

#include "cxx-langstat/Analyses/ClassInstantiationAnalysis.h"

//-----------------------------------------------------------------------------

class ConcurrencySupportLibraryAnalysis : public ClassInstantiationAnalysis {
public:
    ConcurrencySupportLibraryAnalysis();
    ~ConcurrencySupportLibraryAnalysis(){
        std::cout << "CSLA dtor\n";
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void processFeatures(nlohmann::ordered_json j) override;

    const std::string ConcurrencyPrevalenceKey = "concurrency type prevalence";
    const std::string ConcurrencyTypesPrevalenceKey = "concurrency instantiation type arguments";

    static constexpr auto ShorthandName = "csla";
};

//-----------------------------------------------------------------------------

#endif // CONCURRENCYSUPPORTLIBRARYANALYSIS_H
