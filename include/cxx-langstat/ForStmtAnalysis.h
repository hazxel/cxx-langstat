#ifndef FORSTMTANALYSIS_H
#define FORSTMTANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

// need analysis object since we want other analysis to inherit interface
class ForStmtAnalysis : public Analysis {
public:
    ForStmtAnalysis(clang::tooling::ClangTool Tool, int MaxDepthOption);
    // step 0: createMatcher(s)/getMatchers? idea is to have library of often-used
    // matchers or maybe even some datastructure of important matches itself
    // step 1: extraction
    void extract();
    //step 2: compute stats
    void analyzeDepth(Matches<clang::Stmt> matches, std::vector<Matches<clang::Stmt>> Data);
    void analyzeLoopPrevalences(
        Matches<clang::Stmt> fs,
        Matches<clang::Stmt> ws,
        Matches<clang::Stmt> ds);
    //step 3: visualization (for later)
    // combine
    void run();
    // std::string name;
private:
    int MaxDepth;
};

//-----------------------------------------------------------------------------

#endif /* FORSTMTANALYSIS_H */
