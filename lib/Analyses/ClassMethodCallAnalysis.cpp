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
    names_(names), header_regex_(header_regex) {
    std::cout << "CMA ctor" << std::endl;
}

//-----------------------------------------------------------------------------

void ClassMethodCallAnalysis::extractFeatures(){
    auto methodcallmatcher = cxxMemberCallExpr(on(hasType(cxxRecordDecl(names_, isExpansionInFileMatching(header_regex_))))).bind("MethodCall");
    auto results = Extractor.extract2(*Context, methodcallmatcher);
    methodcalls_ = getASTNodes<CXXMemberCallExpr>(results, "MethodCall");
}


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

        // filter out method calls in library files
        if (strstr(file_name, "llvm") != nullptr) {
            continue;
        }

        ordered_json j;
        j[feature_method_name_key_] = called_func;
        j[feature_file_name_key_] = file_name;
        j[feature_line_num_key_] = line_num;
        js[callee_type].emplace_back(j);
    }
    Features[method_call_key_] = js;
}


void ClassMethodCallAnalysis::analyzeFeatures(){
    extractFeatures();
    gatherData(methodcalls_);
}


void ClassMethodCallAnalysis::processFeatures(nlohmann::ordered_json j){
    if(j.contains(method_call_key_)){
        ordered_json res;
        funcPrevalence(j.at(method_call_key_), res);
        Statistics[method_call_key_] = res;
    }
}

void ClassMethodCallAnalysis::funcPrevalence(const ordered_json& in, ordered_json& res){
    std::map<std::string, std::map<std::string, unsigned>> m;
    for (auto& [type_name, func_list] : in.items()) {
        for (auto& func : func_list) {
            m[type_name][func[feature_method_name_key_]]++;
        }
    }
    res = m;
}

//-----------------------------------------------------------------------------

