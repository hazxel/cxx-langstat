#include "cxx-langstat/RecursiveASTCollectVisitor.h"
#include "cxx-langstat/Utils.h"

using namespace clang;

RecursiveASTCollectVisitor::RecursiveASTCollectVisitor(ASTContext *Context) : Context(Context), lo_(), pp_(lo_) {
    pp_.PrintCanonicalTypes = true;
    pp_.SuppressTagKeyword = false;
    pp_.SuppressScope = false;
    pp_.SuppressUnwrittenScope = true;
    pp_.FullyQualifiedName = true;
    pp_.Bool = true;
}

bool RecursiveASTCollectVisitor::VisitVarDecl(VarDecl *Var) {
    if (Var->getType().getAsString(pp_) == currentInstanceName_) {
        hasFoundInstance = true;
    }
    return true;
}

bool RecursiveASTCollectVisitor::VisitCXXMemberCalExpr(CXXMemberCallExpr *Call) {
    if (!hasFoundInstance)
        return true;
    
    std::string callee_type = Call->getObjectType().getAsString();
    std::string called_func = Call->getMethodDecl()->getNameAsString();

    unsigned line_num = Context->getSourceManager().getSpellingLineNumber(Call->getExprLoc());
    auto persumed_loc = Context->getSourceManager().getPresumedLoc(Call->getExprLoc());
    const char* file_name = persumed_loc.getFilename();

    if (isDependentHeader(file_name) || (!isInterestingType(callee_type)))
        return true;

    nlohmann::ordered_json j;
    j[feature_member_method_name_key_] = called_func;
    j[feature_file_name_key_] = file_name;
    j[feature_line_num_key_] = line_num;

    features_[currentInstanceName_][feature_member_method_][callee_type].emplace_back(j);
    return true;
}

bool RecursiveASTCollectVisitor::VisitCallExpr(CallExpr *Call) {
    if (!hasFoundInstance)
        return true;

    if (auto c = llvm::dyn_cast<CXXMemberCallExpr>(Call))
        return VisitCXXMemberCalExpr(c);

    auto callee = Call->getDirectCallee();
    if (!callee)
        return true;

    std::string function_name = callee->getQualifiedNameAsString();
    unsigned line_num = Context->getSourceManager().getSpellingLineNumber(Call->getExprLoc());
    auto persumed_loc = Context->getSourceManager().getPresumedLoc(Call->getExprLoc());
    const char* file_name = persumed_loc.getFilename();

    if (isDependentHeader(file_name) || (!isInterestingFunc(function_name)))
        return true;

    nlohmann::ordered_json j;
    j[feature_function_name_key_] = function_name;
    j[feature_file_name_key_] = file_name;
    j[feature_line_num_key_] = line_num;

    features_[currentInstanceName_][feature_function_].emplace_back(j);
    return true;
}