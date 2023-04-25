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
using ordered_json = nlohmann::ordered_json;
using StringRef = llvm::StringRef;
template<typename T>
using StringMap = std::map<std::string, T>;


ClassInstantiationAnalysis::ClassInstantiationAnalysis() :
    ClassInstantiationAnalysis(anything(), ".*") {
}


ClassInstantiationAnalysis::ClassInstantiationAnalysis(
    internal::Matcher<clang::NamedDecl> Names, std::string HeaderRegex) :
    Names(Names), HeaderRegex(HeaderRegex) {
    //    std::cout << "CIA ctor\n";
}


void ClassInstantiationAnalysis::extractFeatures() {
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
