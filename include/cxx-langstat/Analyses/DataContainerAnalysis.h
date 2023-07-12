
#ifndef DATACONTAINERANALYSIS_H
#define DATACONTAINERANALYSIS_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Deduplicator.h"

//-----------------------------------------------------------------------------

class DataContainerAnalysis : public Analysis {
public:
    DataContainerAnalysis() : lo_(), pp_(lo_) {
        pp_.PrintCanonicalTypes = true;
        pp_.SuppressTagKeyword = false;
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
    static constexpr auto ShorthandName = "dca";

    const std::string feature_container_usage_key_ = "container_usage";
    const std::string method_name_key_ = "called_method";
    const std::string file_name_key_ = "file_name";
    const std::string line_num_key_ = "line_num";
};

//-----------------------------------------------------------------------------

#endif // DATACONTAINERANALYSIS_H
