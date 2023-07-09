
#ifndef SCOPEANALYSIS_H
#define SCOPEANALYSIS_H

#include "clang/ASTMatchers/ASTMatchersInternal.h"

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Deduplicator.h"

//-----------------------------------------------------------------------------

class ScopeAnalysis : public Analysis {
public:
    ScopeAnalysis(
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> names = clang::ast_matchers::anything(), 
        std::string headers = ".*"
    ) : names_(names), header_regex_(headers) {}

    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;

    static Deduplicator deduplicator_;

    clang::ast_matchers::internal::Matcher<clang::NamedDecl> names_;
    std::string header_regex_ = "mutex|bits/std_mutex.h";

    static constexpr auto ShorthandName = "sa";
    static constexpr auto scope_key_ = "scope";
};

//-----------------------------------------------------------------------------

#endif // DOMAINANALYSIS_H
