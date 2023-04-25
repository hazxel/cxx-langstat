
#ifndef CLASSINSTANTIATIONANALYSIS_H
#define CLASSINSTANTIATIONANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class ClassInstantiationAnalysis : public Analysis {
public:
    ClassInstantiationAnalysis();
    ClassInstantiationAnalysis(
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names,
        std::string HeaderRegex);
    ~ClassInstantiationAnalysis(){
        //std::cout << "CIA dtor\n";
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names;
    // Declarations that declare a variable of type from the list above.
    Matches<clang::DeclaratorDecl> Variables;

    void extractFeatures();
    void gatherData(const Matches<clang::DeclaratorDecl>& Matches);
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    
    std::string HeaderRegex;
    private:static constexpr auto ShorthandName = "cia";

protected:
    // JSON keys
    const std::string VarKey = "var insts";

};


//-----------------------------------------------------------------------------

#endif // CLASSINSTANTIATIONANALYSIS_H