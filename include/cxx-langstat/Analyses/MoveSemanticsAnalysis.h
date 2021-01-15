#ifndef MOVESEMANTICSANALYSIS_H
#define MOVESEMANTICSANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class MoveSemanticsAnalysis : public Analysis {
public:
    MoveSemanticsAnalysis(){
        std::cout << "MSA ctor\n";
    }
    ~MoveSemanticsAnalysis(){
        std::cout << "MSA dtor\n";
    }
private:
    void extractFeatures();
    void analyzeFeatures() override;
    void processJSON() override;
    template<typename T>
    void gatherData(std::string DeclKind, std::string PassKind,
        const Matches<T>& Matches);
    void addFunction(const Match<clang::FunctionDecl>& match,
        std::map<std::string, bool> ParmMap);
    void addFunction(const Match<clang::FunctionTemplateDecl>& match,
        std::map<std::string, bool> ParmMap);
    template<typename T>
    void associateParameters(const Matches<T>& Matches);
    Matches<clang::FunctionDecl> FuncsWithValueParm;
    Matches<clang::FunctionDecl> FuncsWithNonConstLValueRefParm;
    Matches<clang::FunctionDecl> FuncsWithConstLValueRefParm;
    Matches<clang::FunctionDecl> FuncsWithRValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithValueParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithNonConstLValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithConstLValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithRValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithUniversalRefParm;
    Matches<clang::ParmVarDecl> ValueParms;
    Matches<clang::ParmVarDecl> NonConstLValueRefParms;
    Matches<clang::ParmVarDecl> ConstLValueRefParms;
    Matches<clang::ParmVarDecl> RValueRefParms;
    Matches<clang::ParmVarDecl> UniversalRefParms;
};

#endif // MOVESEMANTICSANALYSIS_H

//-----------------------------------------------------------------------------
