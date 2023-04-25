#include <iostream>
#include <vector>

#include "cxx-langstat/Analyses/ContainerLibAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
template<typename T>
using StringMap = std::map<std::string, T>;

//-----------------------------------------------------------------------------
// How often were constructs from standard library used (like vector, array,
// map, list, unordered_map, set etc.). Were they used directly as type,
// or as part of another constructs? What behavior can we see when they are
// passed around? What sizes do they occur (#elements, constexpr)?
// Usage in templates and TMP?

// Construct a CLA by constructing a more constrained TIA.
ContainerLibAnalysis::ContainerLibAnalysis() : TemplateInstantiationAnalysis(
    InstKind::Class,
    hasAnyName(
        // Standard library containers, Copied from UseAutoCheck.cpp
        "std::array", "std::vector", "std::deque",
        "std::forward_list", "std::list",
        "std::set", "std::map",
        "std::multiset", "std::multimap",
        "std::unordered_set", "std::unordered_map",
        "std::unordered_multiset", "std::unordered_multimap",
        "std::stack", "std::queue", "std::priority_queue"
    ),
    // libc++
    "array|vector|deque|forward_list|list|set|map|unordered_set|unordered_map|"
    "stack|queue"){
    // no libstdc++ header necessary?
    std::cout << "CLA ctor\n";
}

namespace {

// Map that for a stdlib type contains how many instantiation type args are
// intersting to us, e.g. for the instantiation stored in a .json below,
// "std::vector" : {
//     "location": "42"
//     "arguments": {
//         "non-type": [],
//         "type": [
//             "int",
//             "std::allocator<int>"
//         ],
//         "template": []
//     }
// }
// the result is 1, since we only care about the fact that std::vector was
// instantiated with int.
// -1 indicates that we want all arguments interest us.
// Used to analyze types contained in containers, utilities, smart pointer etc.
const StringMap<int> NumRelTypes = { // no constexpr support for map
    // ** Containers **
    // sequential containers
    {"std::array", 1}, {"std::vector", 1}, {"std::deque", 1},
    {"std::forward_list", 1}, {"std::list", 1},
    // associative containers
    {"std::set", 1}, {"std::map", 2},
    {"std::multiset", 1}, {"std::multimap", 2},
    // Same, but unordered
    {"std::unordered_set", 1}, {"std::unordered_map", 2},
    {"std::unordered_multiset", 1}, {"std::unordered_multimap", 2},
    // container adaptors
    {"std::stack", 1}, {"std::queue", 1}, {"std::priority_queue", 1},
};

} // namespace

// Gathers data on how often each container template was used.
void containerPrevalence(const ordered_json& in, ordered_json& out){
    templatePrevalence(in, out);
}

// For each container template, gives statistics on how often each instantiation
// was used by a (member) variable.
void containertemplateTypeArgPrevalence(const ordered_json& in, ordered_json& out){
    templateTypeArgPrevalence(in, out, NumRelTypes);
}

void ContainerLibAnalysis::processFeatures(ordered_json j){
    if(j.contains(ImplicitClassKey)){
        ordered_json res1;
        ordered_json res2;
        containerPrevalence(j.at(ImplicitClassKey), res1);
        containertemplateTypeArgPrevalence(j.at(ImplicitClassKey), res2);
        Statistics[ContainerPrevalenceKey] = res1;
        Statistics[ContainedTypesPrevalenceKey] = res2;
    }
}

//-----------------------------------------------------------------------------
