#include "cxx-langstat/RecursiveASTCollectVisitor.h"

using namespace clang;

bool RecursiveASTCollectVisitor::VisitVarDecl(VarDecl *Var) {
    if (Var->getType().getAsString() == currentInstanceName_) {
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

    std::string type = callee->getQualifiedNameAsString();
    unsigned line_num = Context->getSourceManager().getSpellingLineNumber(Call->getExprLoc());
    auto persumed_loc = Context->getSourceManager().getPresumedLoc(Call->getExprLoc());
    const char* file_name = persumed_loc.getFilename();

    nlohmann::ordered_json j;
    j[feature_function_name_key_] = type;
    j[feature_file_name_key_] = file_name;
    j[feature_line_num_key_] = line_num;

    features_[currentInstanceName_][feature_function_].emplace_back(j);
    return true;
}