#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include "cxx-langstat/Analyses/ClassMethodCallAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;
using std::string;
using std::vector;
using std::cout;
using std::endl;

// FIXME:
// Use nontrivial destructor if necessary
ClassMethodCallAnalysis::ClassMethodCallAnalysis() :
    ClassMethodCallAnalysis(anything(), ".*") {}


ClassMethodCallAnalysis::ClassMethodCallAnalysis(
    internal::Matcher<clang::NamedDecl> names, std::string header_regex) :
    names_(names), header_regex_(header_regex) {}

//-----------------------------------------------------------------------------

void ClassMethodCallAnalysis::extractFeatures(){
    auto methodcallmatcher = cxxMemberCallExpr(on(hasType(cxxRecordDecl(names_, isExpansionInFileMatching(header_regex_))))).bind("MethodCall");
    methodcalls_ = getASTNodes<CXXMemberCallExpr>(Extractor.extract2(*Context, methodcallmatcher), "MethodCall");

    auto constructorcallmatcher = varDecl(has(cxxConstructExpr()), hasType(cxxRecordDecl(names_, isExpansionInFileMatching(header_regex_)))).bind("ConstructorCall");
    constructorcalls_ = getASTNodes<VarDecl>(Extractor.extract2(*Context, constructorcallmatcher), "ConstructorCall");

    auto functioncallmatcher = callExpr(callee(functionDecl(names_, isExpansionInFileMatching(header_regex_)))).bind("FunctionCall");
    // auto functioncallmatcher = callExpr(isExpansionInFileMatching(header_regex_)).bind("FunctionCall");
    functioncalls_ = getASTNodes<CallExpr>(Extractor.extract2(*Context, functioncallmatcher), "FunctionCall");
}


// for method calls
void ClassMethodCallAnalysis::gatherData(const Matches<clang::CXXMemberCallExpr>& matches){
    ordered_json js;

    LangOptions lo;
    PrintingPolicy pp(lo);
    pp.PrintCanonicalTypes = true;
    pp.SuppressTagKeyword = false;
    pp.SuppressScope = false;
    pp.SuppressUnwrittenScope = true;
    pp.FullyQualifiedName = true;
    pp.Bool = true;

    for (auto match : matches) {
        string callee_type = match.Node->getObjectType().getAsString(pp);
        string called_func = match.Node->getMethodDecl()->getQualifiedNameAsString();
        unsigned line_num = Context->getSourceManager().getSpellingLineNumber(match.Node->getExprLoc());
        auto persumed_loc = Context->getSourceManager().getPresumedLoc(match.Node->getExprLoc());
        const char* file_name = persumed_loc.getFilename();

        ordered_json j;
        j[feature_method_name_key_] = called_func;
        j[feature_file_name_key_] = file_name;
        j[feature_line_num_key_] = line_num;
        js[callee_type].emplace_back(j);
    }
    Features[method_call_key_] = js;
}

// for constructor calls
void ClassMethodCallAnalysis::gatherData(const Matches<clang::VarDecl>& matches) {
    ordered_json js;

    LangOptions lo;
    PrintingPolicy pp(lo);
    pp.PrintCanonicalTypes = true;
    pp.SuppressTagKeyword = false;
    pp.SuppressScope = false;
    pp.SuppressUnwrittenScope = true;
    pp.FullyQualifiedName = true;
    pp.Bool = true;

    for (auto match : matches) {
        string callee_type = match.Node->getType().getAsString(pp);
        unsigned line_num = Context->getSourceManager().getSpellingLineNumber(match.Node->getLocation());
        auto persumed_loc = Context->getSourceManager().getPresumedLoc(match.Node->getLocation());
        const char* file_name = persumed_loc.getFilename();

        ordered_json j;
        j[feature_file_name_key_] = file_name;
        j[feature_line_num_key_] = line_num;
        js[callee_type].emplace_back(j);
    }
    Features[constructor_call_key_] = js;
}

// for function calls
void ClassMethodCallAnalysis::gatherData(const Matches<clang::CallExpr>& matches) {
    ordered_json js;

    LangOptions lo;
    PrintingPolicy pp(lo);
    pp.PrintCanonicalTypes = true;
    pp.SuppressTagKeyword = false;
    pp.SuppressScope = false;
    pp.SuppressUnwrittenScope = true;
    pp.FullyQualifiedName = true;
    pp.Bool = true;

    for (auto match : matches) {
        auto callee = match.Node->getDirectCallee();
        if (callee == nullptr) {
            continue;
        }
        string type = callee->getNameAsString(); 
        unsigned line_num = Context->getSourceManager().getSpellingLineNumber(match.Node->getExprLoc());
        auto persumed_loc = Context->getSourceManager().getPresumedLoc(match.Node->getExprLoc());
        const char* file_name = persumed_loc.getFilename();

        ordered_json j;
        j[feature_method_name_key_] = type;
        j[feature_file_name_key_] = file_name;
        j[feature_line_num_key_] = line_num;
        js[type].emplace_back(j);
    }
    Features[function_call_key_] = js;
}


void ClassMethodCallAnalysis::analyzeFeatures(){
    extractFeatures();
    gatherData(constructorcalls_);
    gatherData(methodcalls_);
    gatherData(functioncalls_);
}


void ClassMethodCallAnalysis::processFeatures(nlohmann::ordered_json j){
    if(j.contains(constructor_call_key_)){
        ordered_json res;
        constructorPrevalence(j.at(constructor_call_key_), res);
        Statistics[constructor_call_key_] = res;
    }

    if(j.contains(method_call_key_)){
        ordered_json res;
        methodPrevalence(j.at(method_call_key_), res);
        Statistics[method_call_key_] = res;
    }

    if(j.contains(function_call_key_)){
        ordered_json res;
        funcPrevalence(j.at(function_call_key_), res);
        Statistics[function_call_key_] = res;
    }
}

void ClassMethodCallAnalysis::funcPrevalence(const ordered_json& in, ordered_json& res){
    std::map<std::string, unsigned> m;
    for (auto& [func_name, func_list] : in.items()) {
        for (auto& func : func_list) {
            if (func[feature_file_name_key_].get<string>().find("llvm") != string::npos
             || func[feature_file_name_key_].get<string>().find("gcc") != string::npos
             || func[feature_file_name_key_].get<string>().find("homebrew") != string::npos) {
                continue;
            }
            m[func[feature_method_name_key_]]++;
        }
    }
    res = m;
}

void ClassMethodCallAnalysis::methodPrevalence(const ordered_json& in, ordered_json& res){
    std::map<std::string, std::map<std::string, unsigned>> m;
    for (auto& [type_name, func_list] : in.items()) {
        for (auto& func : func_list) {
            if (func[feature_file_name_key_].get<string>().find("llvm") != string::npos
             || func[feature_file_name_key_].get<string>().find("gcc") != string::npos
             || func[feature_file_name_key_].get<string>().find("homebrew") != string::npos) {
                continue;
            }
            m[type_name][func[feature_method_name_key_]]++;
        }
    }
    res = m;
}

void ClassMethodCallAnalysis::constructorPrevalence(const ordered_json& in, ordered_json& res){
    std::map<std::string, int> m;
    for (auto& [type_name, call_list] : in.items()) {
        for (auto& call : call_list) {
            if (call[feature_file_name_key_].get<string>().find("llvm") == string::npos
             || call[feature_file_name_key_].get<string>().find("gcc") == string::npos
             || call[feature_file_name_key_].get<string>().find("homebrew") == string::npos) {
                continue;
            }
            m[type_name]++;
        }
    }
    res = m;
}

//-----------------------------------------------------------------------------

