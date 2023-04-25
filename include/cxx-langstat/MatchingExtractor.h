#ifndef MATCHINGEXTRACTOR_H
#define MATCHINGEXTRACTOR_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <string>

// Helper class to abstract MatchCallback away to function that analyses can
// call to extract MatchResults. Analyses should then apply getASTNodes
// with the bound id to get the actual nodes in the AST.
// In the future, might be more handy to implement MatchCallback directly inside
// of the concrete analyses instead of hiding it after the BaseExtractor interface.

//-----------------------------------------------------------------------------
// Match object to contain data extracted from AST
// contains location, pointer to object representing stmt, decl, or type in AST
// and the ASTContext
template<typename T>
struct Match {
    Match<T>(unsigned Location, const T* Node) :
        Location(Location),
        Node(Node){
    }
    unsigned Location;
    const T* Node;
    bool operator==(Match<T> m){
        return (Location == m.Location && Node == m.Node);
    }
    // https://stackoverflow.com/questions/2758080/how-to-sort-an-stl-vector
    bool operator<(const Match<T>& other) const{
        return (Location < other.Location ||
            (Location == other.Location && Node < other.Node));
    }
    // If T is of type clang::Decl, and can be cast to clang::NamedDecl
    std::string getDeclName(const clang::PrintingPolicy& Policy);
};

template<typename T>
std::string Match<T>::getDeclName(const clang::PrintingPolicy& Policy){
    if(const auto n = dyn_cast<clang::NamedDecl>(this->Node)){
        std::string Result;
        llvm::raw_string_ostream stream(Result);
        bool Qualified = !Policy.SuppressScope;
        // For function decls, FullyQualifiedName overrules SuppressScope
        // https://clang.llvm.org/doxygen/structclang_1_1PrintingPolicy.html#a0abaf247fd6585f3e80aa1923c0edc74
        if(const auto n2 = dyn_cast<clang::FunctionDecl>(this->Node))
            Qualified = Policy.FullyQualifiedName;
        n->getNameForDiagnostic(stream, Policy, Qualified);
        return Result;
    } else {
        assert(false);
    }
}


template<typename T>
using Matches = std::vector<Match<T>>; // allows to do Matches<T>

//-----------------------------------------------------------------------------
// Callback class executed on match
class MatchingExtractor : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    // Run when match is found after extract call with Matcher
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
    std::vector<clang::ast_matchers::MatchFinder::MatchResult> Results;
};

//-----------------------------------------------------------------------------

#endif // MATCHINGEXTRACTOR_H
