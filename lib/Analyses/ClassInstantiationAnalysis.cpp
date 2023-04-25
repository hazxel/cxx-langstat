#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "clang/AST/TemplateBase.h"

#include <nlohmann/json.hpp>

#include "cxx-langstat/Analyses/ClassInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using std::string;
using std::vector;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
using StringRef = llvm::StringRef;
template<typename T>
using StringMap = std::map<std::string, T>;

//-----------------------------------------------------------------------------
// Compute statistics on arguments used to instantiate templates,
// no matter whether instantiation are explicit or implicit.
// Should be divided into 3 categories:
// - classes,
// - functions (including member methods), &
// - variables (including class static member variables, (not
//   class field, those cannot be templated if they're not static))
//
// Template instantiations counted should stem from either explicit instantiations
// written by programmers or from implicit ones through 'natural usage'.
//
// Remember that template parameters can be non-types, types or templates.
// Goal: for each instantiation report:
// - classes:
//   * report every explicit instantiation
//   * report every "implicit instantiation", e.g. report ALL instances of
//     std::vector<int>, notably when that occurs multiple times
// - functions:
//   * report every call of f<t> for some instantiating arguments.
//   * explicit function templates are not reported for brevity and due to time,
//     and ease of implementation
// - variables: Report for each var<t> the instantiation, no matter how often
//   that happened.

ClassInstantiationAnalysis::ClassInstantiationAnalysis() :
    ClassInstantiationAnalysis(anything(), ".*") {
}

ClassInstantiationAnalysis::ClassInstantiationAnalysis(
    internal::Matcher<clang::NamedDecl> Names, std::string HeaderRegex) :
    Names(Names), HeaderRegex(HeaderRegex) {
    //    std::cout << "CIA ctor\n";
}

// test code for instantiations:
// - Test code to see if able to find "subinstantiations"
// classTemplateSpecializationDecl(
//     Names,
//     isTemplateInstantiation(),
//     unless(has(cxxRecordDecl())))
// .bind("ImplicitCTSD"),
// - If variable is inside of a template, then it has to be the
// case that the template is being instantiated
// This doesn't work transitively
// anyOf(unless(hasAncestor(functionTemplateDecl())), isInstantiated()),
// - A variable of CTSD type can be matched by this matcher. For
// some reason if it is, it will match twice. Either you uncomment
// the line down below and disallow those variable instantiations
// to be matched here, or you filter out the duplicates below.
// See VarTemplateInstClass.cpp for insights.
// unless(isTemplateInstantiation())

void ClassInstantiationAnalysis::extractFeatures() {
    // Result of the class insts matcher will give back a pointer to the
    // ClassTemplateSpecializationDecl (CTSD).

    // Want variable that has type of some class instantiation,
    // class name is restricted to come from 'Names'
    // WIP: supporting references/pointer to CTSD too.
    auto classdecl = cxxRecordDecl(Names, isExpansionInFileMatching(HeaderRegex));
    auto typematcher = hasType(hasCanonicalType(hasDeclaration(classdecl)));

    auto ClassInstMatcher = decl(anyOf(
        varDecl(
            isExpansionInMainFile(),
            typematcher)
        .bind("VarsField"),
        // Field declarations (non static variable member)
        fieldDecl(
            isExpansionInMainFile(),
            typematcher)
        .bind("VarsField")));
    auto ClassResults = Extractor.extract2(*Context, ClassInstMatcher);
    // only really needed to find location of where class was implicitly instantiated
    // using variable of member variable/field
    Variables = getASTNodes<DeclaratorDecl>(ClassResults, "VarsField");

    // No doubt, this is a botch. If some class type was instantiated through
    // a variable templates instantiation, which will be matched twice, the loop
    // below will filter out those variable declarations and the belonging
    // implicit class inst with it in O(n^2). A better solution would be to
    // bundle variables with the class template specialization that is their type
    // and sort (O(nlogn)), but for sake of ease this is (still) omitted.
    // That they're matched twice is due to an bug in RecursiveASTVisitor:
    // https://lists.llvm.org/pipermail/cfe-dev/2021-February/067595.html
    // std::cout << Variables.size() << std::endl;
    for(int i=1; i<Variables.size(); i++){
        for(int j=0; j<i; j++){
            if(i!=j && Variables.at(i) == Variables.at(j)){
                Variables.erase(Variables.begin()+i);
                ClassImplicitInsts.erase(ClassImplicitInsts.begin()+i);
                i--;
            }
        }
    }
}


void ClassInstantiationAnalysis::gatherData(const Matches<DeclaratorDecl>& matches) {
    ordered_json js;

    LangOptions LO;
    PrintingPolicy PP(LO);
    PP.PrintCanonicalTypes = true;
    PP.SuppressTagKeyword = false;
    PP.SuppressScope = false;
    PP.SuppressUnwrittenScope = true;
    PP.FullyQualifiedName = true;
    PP.Bool = true;

    for(auto match : matches){
        vector<string> v;
        v.push_back(match.getDeclName(PP));
        v.push_back(match.Node->getDeclKindName());
        // v.push_back(match.Node->getBeginLoc().getL);
        js[match.Node->getType().getAsString(PP)].emplace_back(v);
    }
    Features[VarKey] = js;
}

void ClassInstantiationAnalysis::analyzeFeatures(){
    extractFeatures();
    gatherData(Variables);
}

void ClassInstantiationAnalysis::processFeatures(nlohmann::ordered_json j){
    if(j.contains(VarKey)){
            ordered_json res;
        templatePrevalence(j.at(VarKey), res);
        Statistics[VarKey] = res;
    }
}

//-----------------------------------------------------------------------------
