
#ifndef DATACONTAINERANALYSIS_H
#define DATACONTAINERANALYSIS_H

#include "cxx-langstat/Analyses/ClassMethodCallAnalysis.h"

//-----------------------------------------------------------------------------

class DataContainerAnalysis : public ClassMethodCallAnalysis {
public:
    DataContainerAnalysis();
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    static constexpr auto ShorthandName = "dca";
};

//-----------------------------------------------------------------------------

#endif // DATACONTAINERANALYSIS_H
