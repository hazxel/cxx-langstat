
#ifndef OMPEXECUTABLEDIRECTIVETRANSLATIONUNITANALYSIS_H
#define OMPEXECUTABLEDIRECTIVETRANSLATIONUNITANALYSIS_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Deduplicator.h"


//-----------------------------------------------------------------------------

class OMPExecutableDirectiveTranslationUnitAnalysis : public Analysis {
public:
    OMPExecutableDirectiveTranslationUnitAnalysis();
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;

    static Deduplicator deduplicator_;
    static constexpr auto ShorthandName = "ompedatu";

protected:
    const std::string omp_directive_file_name_key_ = "file_name";
    const std::string omp_directive_key_ = "omp_directive";
};

//-----------------------------------------------------------------------------

#endif // OMPEXECUTABLEDIRECTIVETRANSLATIONUNITANALYSIS_H
