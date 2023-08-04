
#include "cxx-langstat/Analyses/DataContainerTranslationUnitAnalysis.h"
#include "cxx-langstat/Utils.h"

#include <set>
#include <string>

#define CONTAINER_NAMES \
    "std::array", "std::vector", "std::deque", "std::forward_list", "std::list", \
    "std::set", "std::multiset", "std::unordered_set", "std::unordered_multiset", \
    "std::map", "std::multimap", "std::unordered_map", "std::unordered_multimap", \
    "std::stack", "std::queue", "std::priority_queue" \
    "tbb::concurrent_vector", "tbb::concurrent_unordered_map", "tbb::concurrent_unordered_set", "tbb::concurrent_queue", \
    "concurrency::concurrent_vector", "concurrency::concurrent_queue", "concurrency::concurrent_stack", \ 
    "concurrency::concurrent_unordered_map", "concurrency::concurrent_unordered_multimap"\
    "boost::array", "boost::circular_buffer", "boost::container::flat_map", "boost::container::flat_set", \
    "boost::container::list", "boost::container::map", "boost::container::set", "boost::container::slist", \
    "boost::container::stable_vector", "boost::container::static_vector", "boost::container::vector", \
    "boost::heap::binomial_heap", "boost::heap::binomial_heap", "boost::heap::d_ary_heap", \
    "boost::heap::fibonacci_heap", "boost::heap::fibonacci_heap", "boost::heap::pairing_heap", \
    "boost::heap::pairing_heap", "boost::heap::skew_heap", "boost::heap::skew_heap", \
    "boost::heap::priority_queue", "boost::heap::priority_queue", "boost::heap::priority_queue", \
    "boost::lockfree::queue", "boost::lockfree::stack", "boost::lockfree::spsc_queue", \
    "absl::btree_map", "absl::btree_multimap", "absl::btree_multiset", "absl::btree_set", \
    "absl::flat_hash_map", "absl::flat_hash_set", "absl::flat_hash_set", "absl::flat_hash_set", \
    "absl::flat_hash_set", "absl::flat_hash_set", "absl::flat_hash_set", "absl::flat_hash_set", \
    "absl::flat_hash_set", "absl::flat_hash_set", "absl::flat_hash_set", "absl::flat_hash_set", \
    "folly::AtomicHashMap", "folly::AtomicHashArray", "folly::AtomicLinkedList", "folly::AtomicUnorderedMap"




#define CONTAINER_HEADERS \
    "vector|list|forward_list|array|map|set|unordered_map|unordered_set|queue|deque|priority_queue|stack|"\
    "tbb/concurrent_vector.h|tbb/concurrent_unordered_map.h|tbb/concurrent_unordered_set.h|tbb/concurrent_queue.h|"\
    "concurrent_vector.h|concurrent_queue.h|concurrent_stack.h|concurrent_unordered_map.h|concurrent_unordered_multimap.h|"\
    "boost/array.hpp|boost/circular_buffer.hpp|boost/container/flat_map.hpp|boost/container/flat_set.hpp|"\
    "boost/container/list.hpp|boost/container/map.hpp|boost/container/set.hpp|boost/container/slist.hpp|"\
    "boost/container/stable_vector.hpp|boost/container/static_vector.hpp|boost/container/vector.hpp|"\
    "boost/heap/binomial_heap.hpp|boost/heap/binomial_heap.hpp|boost/heap/d_ary_heap.hpp|boost/heap/fibonacci_heap.hpp|"\
    "boost/heap/fibonacci_heap.hpp|boost/heap/pairing_heap.hpp|boost/heap/pairing_heap.hpp|boost/heap/skew_heap.hpp|"\
    "boost/heap/skew_heap.hpp|boost/heap/priority_queue.hpp|boost/heap/priority_queue.hpp|boost/heap/priority_queue.hpp|"\
    "boost/lockfree/queue.hpp|boost/lockfree/stack.hpp|boost/lockfree/spsc_queue.hpp|"\
    "absl/container/btree_map.h|absl/container/btree_multimap.h|absl/container/btree_multiset.h|absl/container/btree_set.h|"\
    "absl/container/flat_hash_map.h|absl/container/flat_hash_set.h|absl/container/flat_hash_set.h|absl/container/flat_hash_set.h|"\
    "absl/container/flat_hash_set.h|absl/container/flat_hash_set.h|absl/container/flat_hash_set.h|absl/container/flat_hash_set.h|"\
    "folly/AtomicHashMap.h|folly/AtomicHashArray.h|folly/AtomicLinkedList.h|folly/AtomicUnorderedMap.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

auto DataContainerTranslationUnitAnalysis::deduplicator_ = Deduplicator();

void DataContainerTranslationUnitAnalysis::analyzeFeatures() {
    auto names_ = clang::ast_matchers::hasAnyName(CONTAINER_NAMES);
    std::string header_regex_ = CONTAINER_HEADERS;
    auto methodcallmatcher = cxxMemberCallExpr(on(hasType(cxxRecordDecl(names_, isExpansionInFileMatching(header_regex_))))).bind("MethodCall");
    auto methodcalls_ = getASTNodes<CXXMemberCallExpr>(Extractor.extract2(*Context, methodcallmatcher), "MethodCall");
    auto typedefmethodcallmatcher = cxxMemberCallExpr(on(hasType(typedefDecl(names_, isExpansionInFileMatching(header_regex_))))).bind("TypedefMethodCall");
    auto typedefmethodcalls = getASTNodes<CXXMemberCallExpr>(Extractor.extract2(*Context, typedefmethodcallmatcher), "TypedefMethodCall");
    methodcalls_.insert(methodcalls_.end(), typedefmethodcalls.begin(), typedefmethodcalls.end());

    auto operatormatcher = cxxOperatorCallExpr(hasOverloadedOperatorName("[]")).bind("OperatorCall");
    auto operatorcalls = getASTNodes<CXXOperatorCallExpr>(Extractor.extract2(*Context, operatormatcher), "OperatorCall");

    ordered_json js;

    for (auto match : methodcalls_) {
        std::string callee_type;
        llvm::raw_string_ostream rso(callee_type);
        match.Node->getRecordDecl()->printQualifiedName(rso, pp_);
        std::string called_func = match.Node->getMethodDecl()->getNameAsString();
        auto persumed_loc = Context->getSourceManager().getPresumedLoc(match.Node->getExprLoc());
        const char* file_name = persumed_loc.getFilename();

        if(isDependentHeader(file_name))
            continue;

        ordered_json j;
        j[method_name_key_] = called_func;
        j[file_name_key_] = file_name;
        js[callee_type].emplace_back(j);
    }

    const std::set<std::string> interesting_names = {CONTAINER_NAMES};
    for (auto match : operatorcalls) {
        std::string callee_type;
        llvm::raw_string_ostream rso(callee_type);
        match.Node->getArg(0)->getBestDynamicClassType()->printQualifiedName(rso, pp_); // should always have at least one arg     
        if (interesting_names.find(callee_type) == interesting_names.end()) {
            continue;
        }
        
        auto persumed_loc = Context->getSourceManager().getPresumedLoc(match.Node->getExprLoc());
        const char* file_name = persumed_loc.getFilename();

        if (isDependentHeader(file_name))
            continue;

        ordered_json j;
        j[method_name_key_] = "operator[]";
        j[file_name_key_] = file_name;
        js[callee_type].emplace_back(j);
    }

    Features[feature_container_usage_key_] = js;
}

void DataContainerTranslationUnitAnalysis::processFeatures(nlohmann::ordered_json j) {
    nlohmann::ordered_json js(nlohmann::detail::value_t::object);
    for (auto& [containername, calls] : j.at(feature_container_usage_key_).items()) {
        std::map<std::string, unsigned> method_call_count;
        for (auto& call : calls) {
            std::string method_name = call.at(method_name_key_);
            std::string file_name = call.at(file_name_key_);

            if (deduplicator_.isDuplicated(file_name + method_name))
                continue;
            method_call_count[method_name]++;
        }
        js[containername] = method_call_count;
    }

    Statistics[feature_container_usage_key_] = js;
}