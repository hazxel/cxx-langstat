
#include "cxx-langstat/Analyses/DataContainerAnalysis.h"
#include "cxx-langstat/Utils.h"

#include <string>

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

auto DataContainerAnalysis::deduplicator_ = Deduplicator();

void DataContainerAnalysis::analyzeFeatures() {
    auto names_ = clang::ast_matchers::hasAnyName(
        "std::array", "std::vector", "std::deque",
        "std::forward_list", "std::list",
        "std::set", "std::map",
        "std::multiset", "std::multimap",
        "std::unordered_set", "std::unordered_map",
        "std::unordered_multiset", "std::unordered_multimap",
        "std::stack", "std::queue", "std::priority_queue"
        // TBB
        "tbb::concurrent_vector", "tbb::concurrent_unordered_map", "tbb::concurrent_unordered_set", "tbb::concurrent_queue",
        // PPL
        "concurrency::concurrent_vector", "concurrency::concurrent_queue", "concurrency::concurrent_stack",
        "concurrency::concurrent_unordered_map", "concurrency::concurrent_unordered_multimap"
    );
    std::string header_regex_ = 
        "vector|list|forward_list|array|map|set|unordered_map|unordered_set|queue|deque|priority_queue|stack|"
        "tbb/concurrent_vector.h|tbb/concurrent_unordered_map.h|tbb/concurrent_unordered_set.h|tbb/concurrent_queue.h|"
        "concurrent_vector.h|concurrent_queue.h|concurrent_stack.h|concurrent_unordered_map.h|concurrent_unordered_multimap.h";
    auto methodcallmatcher = cxxMemberCallExpr(on(hasType(cxxRecordDecl(names_, isExpansionInFileMatching(header_regex_))))).bind("MethodCall");
    auto methodcalls_ = getASTNodes<CXXMemberCallExpr>(Extractor.extract2(*Context, methodcallmatcher), "MethodCall");
    auto typedefmethodcallmatcher = cxxMemberCallExpr(on(hasType(typedefDecl(names_, isExpansionInFileMatching(header_regex_))))).bind("TypedefMethodCall");
    auto typedefmethodcalls = getASTNodes<CXXMemberCallExpr>(Extractor.extract2(*Context, typedefmethodcallmatcher), "TypedefMethodCall");
    methodcalls_.insert(methodcalls_.end(), typedefmethodcalls.begin(), typedefmethodcalls.end());

    auto operatormatcher = cxxOperatorCallExpr(hasOverloadedOperatorName("[]")).bind("OperatorCall");
    auto operatorcalls = getASTNodes<CXXOperatorCallExpr>(Extractor.extract2(*Context, operatormatcher), "OperatorCall");

    ordered_json js;

    for (auto match : methodcalls_) {
        std::string callee_type = match.Node->getObjectType().getAsString(pp_);
        std::string called_func = match.Node->getMethodDecl()->getQualifiedNameAsString();
        unsigned line_num = Context->getSourceManager().getSpellingLineNumber(match.Node->getExprLoc());
        auto persumed_loc = Context->getSourceManager().getPresumedLoc(match.Node->getExprLoc());
        const char* file_name = persumed_loc.getFilename();

        if(isDependentHeader(file_name))
            continue;

        ordered_json j;
        j[method_name_key_] = called_func;
        j[file_name_key_] = file_name;
        j[line_num_key_] = line_num;
        js[callee_type].emplace_back(j);
    }

    for (auto match : operatorcalls) {
        std::string callee_type = match.Node->getArg(0)->getType().getAsString(pp_); // should always have at least one arg       
        unsigned line_num = Context->getSourceManager().getSpellingLineNumber(match.Node->getExprLoc());
        auto persumed_loc = Context->getSourceManager().getPresumedLoc(match.Node->getExprLoc());
        const char* file_name = persumed_loc.getFilename();

        if (isDependentHeader(file_name))
            continue;

        if (callee_type.find("std::") == std::string::npos
            && callee_type.find("tbb::") == std::string::npos
            && callee_type.find("concurrency::") == std::string::npos) {
            continue;
        }
        
        ordered_json j;
        j[method_name_key_] = "operator[]";
        j[file_name_key_] = file_name;
        j[line_num_key_] = line_num;
        js[callee_type].emplace_back(j);
    }

    Features[feature_container_usage_key_] = js;
}

void DataContainerAnalysis::processFeatures(nlohmann::ordered_json j) {
    nlohmann::ordered_json js(nlohmann::detail::value_t::object);
    for (auto& [containername, calls] : j.at(feature_container_usage_key_).items()) {
        std::map<std::string, unsigned> method_call_count;
        for (auto& call : calls) {
            std::string method_name = call.at(method_name_key_);
            std::string file_name = call.at(file_name_key_);
            unsigned line_num = call.at(line_num_key_);

            if (deduplicator_.isDuplicated(file_name + std::to_string(line_num) + method_name))
                continue;
            method_call_count[method_name]++;
        }
        js[containername] = method_call_count;
    }

    Statistics[feature_container_usage_key_] = js;
}