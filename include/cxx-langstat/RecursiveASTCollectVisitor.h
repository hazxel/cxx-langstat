#pragma once

#include <string>
#include <map>
#include <vector>

#include "nlohmann/json.hpp"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/AST/StmtOpenMP.h"


class RecursiveASTCollectVisitor : public clang::RecursiveASTVisitor<RecursiveASTCollectVisitor> {
public:
    explicit RecursiveASTCollectVisitor(clang::ASTContext *Context);

    bool VisitVarDecl(clang::VarDecl *Var);

    bool VisitCXXMemberCalExpr(clang::CXXMemberCallExpr *Call);

    bool VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr *Call);

    bool VisitCallExpr(clang::CallExpr *Call);

    inline void reset() {
        hasFoundInstance = false;
    }

    inline void setInstanceName(std::string instancename) {
        currentInstanceName_ = instancename;
    }

    inline void disableInstanceCheck() {
        hasFoundInstance = true;
    }

    inline const nlohmann::json getFeatures() {
        return features_;
    }

private:
    inline bool isInterestingType(std::string type) {
        for (auto &t : interestingTypes_) {
            if (type.find(t) != std::string::npos)
                return true;
        }
        return false;
    }

    inline bool isInterestingFunc(std::string func) {
        for (auto &f : interestingFuncs_) {
            if (func.find(f) != std::string::npos)
                return true;
        }
        return false;
    }

    inline bool isInterestingOperator(clang::OverloadedOperatorKind op) {
        for (auto &o : interestingOperators_) {
            if (op == o)
                return true;
        }
        return false;
    }

    inline std::string removeTemplateArgs(std::string type) {
        auto pos = type.find('<');
        if (pos != std::string::npos) {
            type = type.substr(0, pos);
        }
        return type;
    }

private:
    clang::LangOptions lo_;
    clang::PrintingPolicy pp_;

    clang::ASTContext *Context;
    bool hasFoundInstance = false;
    std::string currentInstanceName_;
    std::vector<std::string> interestingTypes_ = {
        "vector", "list", "set", "map", "unordered_map", "unordered_set", "deque", "forward_list", "stack", "queue", "priority_queue"
    };
    std::vector<std::string> interestingFuncs_ = {
        "sort", "move", "free", "init", "atomic", "load", "store", "MPI_"
    };
    std::vector<clang::OverloadedOperatorKind> interestingOperators_ = {
        clang::OO_Subscript
    };

    nlohmann::json features_;
    std::vector<std::map<std::string, std::string>> functionFeatures_;
    std::map<std::string, std::vector<std::map<std::string, std::string>>> memberMethodFeatures_;

public:
    static constexpr auto feature_member_method_ = "member_method_features";
    static constexpr auto feature_function_ = "function_features";

    static constexpr auto feature_member_method_name_key_ = "member_method_name";
    static constexpr auto feature_function_name_key_ = "function_name";
    static constexpr auto feature_file_name_key_ = "file_name";
    static constexpr auto feature_line_num_key_ = "line_num";
};