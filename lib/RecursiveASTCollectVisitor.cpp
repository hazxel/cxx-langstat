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
    
    std::string callee_type = Call->getObjectType().getAsString(pp_);
    std::string called_func = Call->getMethodDecl()->getNameAsString();
    
    callee_type = removeTemplateArgs(callee_type);

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

bool RecursiveASTCollectVisitor::VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr *Call) {
    if (!hasFoundInstance)
        return true;
    
    clang::OverloadedOperatorKind operator_type = Call->getOperator();
    if (!isInterestingOperator(operator_type))
        return true;

    std::string callee_type = Call->getArg(0)->getType().getAsString(pp_); // should always have at least one arg
    callee_type = removeTemplateArgs(callee_type);
    if (!isInterestingType(callee_type))
        return true;
    auto direct_callee = Call->getDirectCallee();
    if (!direct_callee)
        return true;
    std::string operator_name = direct_callee->getNameAsString();

    unsigned line_num = Context->getSourceManager().getSpellingLineNumber(Call->getExprLoc());
    auto persumed_loc = Context->getSourceManager().getPresumedLoc(Call->getExprLoc());
    const char* file_name = persumed_loc.getFilename();

    if (isDependentHeader(file_name) )
        return true;

    nlohmann::ordered_json j;
    j[feature_member_method_name_key_] = operator_name;
    j[feature_file_name_key_] = file_name;
    j[feature_line_num_key_] = line_num;

    features_[currentInstanceName_][feature_member_method_][callee_type].emplace_back(j);
    
    return true;
}

bool RecursiveASTCollectVisitor::VisitCallExpr(CallExpr *Call) {
    if (auto c = llvm::dyn_cast<CXXMemberCallExpr>(Call)) {
        return VisitCXXMemberCalExpr(c);
    }

    auto callee = Call->getDirectCallee();
    if (!callee)
        return true;

    std::string function_name = callee->getQualifiedNameAsString();
    if (function_name == "pthread_mutex_lock") {
        hasFoundInstance = true;
        return true;
    } else if(function_name == "pthread_mutex_unlock") {
        hasFoundInstance = false;
        return true;
    }

    if (!hasFoundInstance)
        return true;

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