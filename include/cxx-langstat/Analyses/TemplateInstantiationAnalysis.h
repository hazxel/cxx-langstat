#ifndef TEMPLATEINSTANTIATIONANALYSIS_H
#define TEMPLATEINSTANTIATIONANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

enum class InstKind {
    Class, Function, Variable, Any
};

class TemplateInstantiationAnalysis : public Analysis {
public:
    // Ctor to let TIA look for any kind of instantiations
    TemplateInstantiationAnalysis();
    // Ctor to instrument TIA to look only for instantiations of kind IK
    // named any of "Names".
    TemplateInstantiationAnalysis(InstKind IK,
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names,
        std::string HeaderRegex);
    ~TemplateInstantiationAnalysis(){}
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    InstKind IK;
    clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names;
    // CTSDs that appeared in explicit instantiation.
    Matches<clang::ClassTemplateSpecializationDecl> ClassImplicitInsts;
    // CTSDs that appeared in implicit instantiation or where used by a variable
    // that has CTSD type.
    Matches<clang::ClassTemplateSpecializationDecl> ClassExplicitInsts;
    // Declarations that declare a variable of type CTSD from the list above.
    Matches<clang::DeclaratorDecl> Variables;
    // Function instantiations referenced by calls.
    Matches<clang::FunctionDecl> FuncInsts;
    // Call exprs that cause an implicit function instantiation or refer to it.
    Matches<clang::CallExpr> Callers;
    //
    Matches<clang::VarTemplateSpecializationDecl> VarInsts;
    // Responsible to fill vectors of matches defined above
    void extractFeatures();
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    // Get location of instantiation
    template<typename T>
    unsigned getInstantiationLocation(const Match<T>& Match, bool isImplicit);
    // Specializations to get locations of class and func template uses.
    template<>
    unsigned getInstantiationLocation
        (const Match<clang::ClassTemplateSpecializationDecl>& Match,
            bool isImplicit);
    template<>
    unsigned getInstantiationLocation(const Match<clang::FunctionDecl>& Match,
            bool isImplicit);
    // Given matches representing the instantiations of some kind, gather
    // for each instantiation the instantiation arguments.
    template<typename T>
    void gatherInstantiationData(Matches<T>& Insts, std::string InstKind,
        bool AreImplicit);
    //
    unsigned VariablesCounter = 0;
    unsigned CallersCounter = 0;
    // Regex specifying the header that the template comes from, i.e. usually
    // where it is *defined*. This is important s.t. when the analyzer looks for
    // instantiations of some template, it is ensured that the instantiation is
    // not a false positive of some other template having the same name. Note
    // that when analyzing C++ library templates you might have to dig
    // deeper to find the file containing the definition of the template, which
    // can be an internal header file you usually don't include yourself.
    std::string HeaderRegex;
protected:
    // JSON keys
    const std::string ExplicitClassKey = "explicit class insts";
    const std::string ImplicitClassKey = "implicit class insts";
    const std::string FuncKey = "func insts";
    const std::string VarKey = "var insts";
    //
    static constexpr auto ShorthandName = "tia";
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
