
#ifndef CONCURRENCYSUPPORTLIBRARYTRANLATIONUNITANALYSIS_H
#define CONCURRENCYSUPPORTLIBRARYTRANLATIONUNITANALYSIS_H

#include "cxx-langstat/Analyses/ClassMethodCallTranslationUnitAnalysis.h"

//-----------------------------------------------------------------------------

class ConcurrencySupportLibraryMethodTranslationUnitAnalysis : public ClassMethodCallTranslationUnitAnalysis {
public:
    ConcurrencySupportLibraryMethodTranslationUnitAnalysis();
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    // void processFeatures(nlohmann::ordered_json j) override;
    static constexpr auto ShorthandName = "cslmatu";
};

//-----------------------------------------------------------------------------

#endif // CONCURRENCYSUPPORTLIBRARYTRANLATIONUNITANALYSIS_H





