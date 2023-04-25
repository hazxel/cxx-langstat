
#ifndef CLASSMETHODCALLANALYSIS_H
#define CLASSMETHODCALLANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class ClassMethodCallAnalysis : public Analysis {
public:
    ClassMethodCallAnalysis(); 
    ClassMethodCallAnalysis(
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names,
        std::string HeaderRegex);
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void extractFeatures();
    void gatherData(const Matches<clang::CXXMemberCallExpr>& matches);
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    void funcPrevalence(const nlohmann::ordered_json &in, nlohmann::ordered_json& res);


    clang::ast_matchers::internal::Matcher<clang::NamedDecl> names_;
    std::string header_regex_;
    Matches<clang::CXXMemberCallExpr> methodcalls_;
    static constexpr auto ShorthandName = "cmca";

protected:
    const std::string method_call_key_ = "method calls";
    const std::string feature_method_name_key_ = "called_method";
    const std::string feature_file_name_key_ = "file_name";
    const std::string feature_line_num_key_ = "line_num";
};

//-----------------------------------------------------------------------------

#endif // CLASSMETHODCALLANALYSIS_H
