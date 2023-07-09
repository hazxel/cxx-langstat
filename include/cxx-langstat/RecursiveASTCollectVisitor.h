#pragma once

#include <string>
#include <map>
#include <vector>

#include "nlohmann/json.hpp"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"


class RecursiveASTCollectVisitor : public clang::RecursiveASTVisitor<RecursiveASTCollectVisitor> {
public:
    explicit RecursiveASTCollectVisitor(clang::ASTContext *Context) : Context(Context) {}

    bool VisitVarDecl(clang::VarDecl *Var);

    bool VisitCXXMemberCalExpr(clang::CXXMemberCallExpr *Call);

    bool VisitCallExpr(clang::CallExpr *Call);

    inline void reset() {
        hasFoundInstance = false;
    }

    inline void setInstanceName(std::string instancename) {
        currentInstanceName_ = instancename;
    }

    // inline const std::unordered_map<std::string, std::vector<std::string>>& getFeatures() {
    //     return features_;
    // }
    inline const nlohmann::json getFeatures() {
        return features_;
    }

    // inline const std::vector<std::map<std::string, std::string>>& getFunctionFeatures() {
    //     return functionFeatures_;
    // }

    // inline const std::map<std::string, std::vector<std::map<std::string, std::string>>>& getMemberMethodFeatures() {
    //     return memberMethodFeatures_;
    // }

private:
    clang::ASTContext *Context;
    bool hasFoundInstance = false;
    std::string currentInstanceName_;
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