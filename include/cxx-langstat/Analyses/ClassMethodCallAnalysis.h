
#ifndef CLASSMETHODCALLANALYSIS_H
#define CLASSMETHODCALLANALYSIS_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Deduplicator.h"

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
    void gatherData(const Matches<clang::VarDecl>& matches);
    void gatherData(const Matches<clang::CallExpr>& matches);
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    void funcPrevalence(const nlohmann::ordered_json &in, nlohmann::ordered_json& res);
    void constructorPrevalence(const nlohmann::ordered_json &in, nlohmann::ordered_json& res);
    void methodPrevalence(const nlohmann::ordered_json &in, nlohmann::ordered_json& res);
    static bool isDuplicated(const std::string& str);

    static Deduplicator deduplicator_;
    clang::ast_matchers::internal::Matcher<clang::NamedDecl> names_;
    std::string header_regex_;
    Matches<clang::CXXMemberCallExpr> methodcalls_;
    Matches<clang::VarDecl> constructorcalls_;
    Matches<clang::CallExpr> functioncalls_;
    static constexpr auto ShorthandName = "cmca";

protected:
    const std::string constructor_call_key_ = "constructor calls";
    const std::string method_call_key_ = "method calls";
    const std::string function_call_key_ = "function calls";
    const std::string feature_method_name_key_ = "called_method";
    const std::string feature_file_name_key_ = "file_name";
    const std::string feature_line_num_key_ = "line_num";
};

//-----------------------------------------------------------------------------

#endif // CLASSMETHODCALLANALYSIS_H
