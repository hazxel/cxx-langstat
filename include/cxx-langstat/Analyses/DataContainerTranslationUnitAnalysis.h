
#ifndef DATACONTAINERTRANSLATIONUNITANALYSIS_H
#define DATACONTAINERTRANSLATIONUNITANALYSIS_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Deduplicator.h"

//-----------------------------------------------------------------------------

class DataContainerTranslationUnitAnalysis : public Analysis {
public:
    DataContainerTranslationUnitAnalysis() : lo_(), pp_(lo_) {
        pp_.PrintCanonicalTypes = true;
        pp_.SuppressTagKeyword = true;
        pp_.SuppressScope = false;
        pp_.SuppressUnwrittenScope = true;
        pp_.FullyQualifiedName = true;
        pp_.Bool = true;
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    clang::LangOptions lo_;
    clang::PrintingPolicy pp_;

    static Deduplicator deduplicator_;

    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    static constexpr auto ShorthandName = "dcatu";

    const std::string feature_container_usage_key_ = "container_usage";
    const std::string method_name_key_ = "called_method";
    const std::string file_name_key_ = "file_name";
};

//-----------------------------------------------------------------------------

#endif // DATACONTAINERTRANSLATIONUNITANALYSIS_H