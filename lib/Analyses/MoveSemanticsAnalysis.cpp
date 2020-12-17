#include <iostream>

#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------

void MoveSemanticsAnalysis::extract(){
    // Writing the matcher like this means that a functionDecl can only bind to
    // one id - good, since it inhibits counting a decl twice under different ids
    DeclarationMatcher functionMatcher = decl(anyOf(
        functionDecl(unless(hasAnyParameter(hasType(referenceType()))))
        .bind("copy"),
        functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(unless(isConstQualified()))))))
        .bind("nonconstlvalue"),
        functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(isConstQualified())))))
        .bind("constlvalue"),
        functionDecl(hasAnyParameter(hasType(rValueReferenceType())))
        .bind("rvalue")
    ));

    DeclarationMatcher parmMatcher = decl(anyOf(
        parmVarDecl(unless(hasType(referenceType())))
        .bind("copy"),
        parmVarDecl(hasType(lValueReferenceType(
                pointee(unless(isConstQualified())))))
        .bind("lvalue"),
        parmVarDecl(hasType(lValueReferenceType(
                pointee(isConstQualified()))))
        .bind("nonconstlvalue"),
        parmVarDecl(hasType(rValueReferenceType()))
        .bind("rvalue")
    ));

    auto functions = Extractor.extract2(*Context, functionMatcher);
    auto parms = Extractor.extract2(*Context, parmMatcher);
    FunctionDeclsWithCopy = getASTNodes<FunctionDecl>(functions, "copy");
    FunctionDeclsWithlValueRef = getASTNodes<FunctionDecl>(functions, "nonconstlvalue");
    FunctionDeclsWithConstlValueRef = getASTNodes<FunctionDecl>(functions, "constlvalue");
    FunctionDeclsWithrValueRef = getASTNodes<FunctionDecl>(functions, "rvalue");
    CopyParmDecls = getASTNodes<ParmVarDecl>(parms,  "copy");
    lValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "nonconstlvalue");
    ConstlValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "constlvalue");
    rValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "rvalue");

}

template<typename T>
void MoveSemanticsAnalysis::gatherStatistics(std::string text,
    const Matches<T>& Matches){
        ordered_json Decls;
        for(auto match : Matches){
            ordered_json d;
            d["location"] = match.location;
            Decls[getMatchDeclName(match)] = d;
        }
        Result[text] = Decls;
}

void MoveSemanticsAnalysis::run(llvm::StringRef InFile,
    clang::ASTContext& Context) {
        std::cout << "\033[32mRunning MSA:\033[0m" << std::endl;
        this->Context = &Context;
        extract();
        gatherStatistics("fdecls with copying", FunctionDeclsWithCopy);
        gatherStatistics("fdecls with non-const lvalue ref", FunctionDeclsWithlValueRef);
        gatherStatistics("fdecls with const lvalue ref", FunctionDeclsWithConstlValueRef);
        gatherStatistics("fdecls with rvalue ref", FunctionDeclsWithrValueRef);
        gatherStatistics("copy parms", CopyParmDecls);
        gatherStatistics("non-const lvalue ref parms", lValueRefParmDecls);
        gatherStatistics("const lvalue ref parms", ConstlValueRefParmDecls);
        gatherStatistics("rvalue parms", rValueRefParmDecls);
}

//-----------------------------------------------------------------------------
