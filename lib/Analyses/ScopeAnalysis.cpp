
#include "cxx-langstat/Analyses/ScopeAnalysis.h"
#include "cxx-langstat/RecursiveASTCollectVisitor.h"
#include "cxx-langstat/Utils.h"

#include <vector>
#include <unordered_map>
#include <map>

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

auto ScopeAnalysis::deduplicator_ = Deduplicator();

void ScopeAnalysis::analyzeFeatures(){    
    // STL like mutex and lock
    auto constructorcallmatcher = varDecl(has(cxxConstructExpr()), hasType(cxxRecordDecl(names_, isExpansionInFileMatching(header_regex_)))).bind("ConstructorCall");
    Matches<clang::VarDecl> constructorcalls_ = getASTNodes<VarDecl>(Extractor.extract2(*Context, constructorcallmatcher), "ConstructorCall");
    auto typedefconstructorcallmatcher = varDecl(has(cxxConstructExpr()), hasType(typedefDecl(names_, isExpansionInFileMatching(header_regex_)))).bind("TypedefConstructorCall");
    auto typedefconstructorcalls = getASTNodes<VarDecl>(Extractor.extract2(*Context, typedefconstructorcallmatcher), "TypedefConstructorCall");
    constructorcalls_.insert(constructorcalls_.end(), typedefconstructorcalls.begin(), typedefconstructorcalls.end());

    LangOptions lo;
    PrintingPolicy pp(lo);
    pp.PrintCanonicalTypes = true;
    pp.SuppressTagKeyword = false;
    pp.SuppressScope = false;
    pp.SuppressUnwrittenScope = true;
    pp.FullyQualifiedName = true;
    pp.Bool = true;

    std::unordered_map<std::string, std::vector<const clang::CompoundStmt*>> enclosingStmtsMap;
    for (auto match : constructorcalls_) {
        std::string instancename = match.Node->getType().getAsString(pp);
        const clang::Stmt* currentStmt = llvm::dyn_cast<clang::Stmt>(Context->getParents(*match.Node).begin()->get<clang::Stmt>());

        while (currentStmt) {
            if (const clang::CompoundStmt* compoundStmt = llvm::dyn_cast<clang::CompoundStmt>(currentStmt)) {
                enclosingStmtsMap[instancename].push_back(compoundStmt);
                break;
            }
            currentStmt = Context->getParents(*currentStmt).begin()->get<clang::Stmt>();
        }
    }

    RecursiveASTCollectVisitor visitor(Context);
    for (auto pair : enclosingStmtsMap) {
        std::string instancename = pair.first;
        std::vector<const clang::CompoundStmt*> enclosingStmts = pair.second;

        visitor.setInstanceName(instancename);

        for (auto stmt : enclosingStmts) {
            visitor.reset();
            visitor.TraverseStmt(const_cast<clang::CompoundStmt*>(stmt));
        }
    }


    // openmp critical block
    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<clang::Stmt, clang::OMPCriticalDirective> ompCriticalDirective;
    auto ompCriticalMatcher = ompCriticalDirective(hasStructuredBlock(stmt())).bind("OMPCriticalBlock");
    auto ompCriticalDirs = getASTNodes<OMPCriticalDirective>(Extractor.extract2(*Context, ompCriticalMatcher), "OMPCriticalBlock");
    visitor.setInstanceName("#pragma omp critical");
    for (auto ompDir : ompCriticalDirs) {
        visitor.disableInstanceCheck();
        visitor.TraverseStmt(const_cast<clang::Stmt*>(ompDir.Node->getStructuredBlock()));
    }

    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<clang::Stmt, clang::OMPAtomicDirective> ompAtomicDirective;
    auto ompAtomicMatcher = ompAtomicDirective(hasStructuredBlock(stmt())).bind("OMPAtomicBlock");
    auto ompAtomicDirs = getASTNodes<OMPAtomicDirective>(Extractor.extract2(*Context, ompAtomicMatcher), "OMPAtomicBlock");
    visitor.setInstanceName("#pragma omp atomic");
    for (auto ompDir : ompAtomicDirs) {
        visitor.disableInstanceCheck();
        visitor.TraverseStmt(const_cast<clang::Stmt*>(ompDir.Node->getStructuredBlock()));
    }

    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<clang::Stmt, clang::OMPBarrierDirective> ompBarrierDirective;
    auto ompBarrierMatcher = ompBarrierDirective().bind("OMPBarrier");
    auto ompBarrierDirs = getASTNodes<OMPBarrierDirective>(Extractor.extract2(*Context, ompBarrierMatcher), "OMPBarrier");
    visitor.setInstanceName("#pragma omp barrier");
    for (auto ompDir : ompBarrierDirs) {
        visitor.disableInstanceCheck();
        visitor.TraverseStmt(const_cast<clang::Stmt*>(ompDir.Node->getStructuredBlock()));
    }

    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<clang::Stmt, clang::OMPTaskwaitDirective> ompTaskwaitDirective;    
    auto ompTaskwaitMatcher = ompTaskwaitDirective().bind("OMPTaskwait");
    auto ompTaskwaitDirs = getASTNodes<OMPTaskwaitDirective>(Extractor.extract2(*Context, ompTaskwaitMatcher), "OMPTaskwait");
    visitor.setInstanceName("#pragma omp taskwait");
    for (auto ompDir : ompTaskwaitDirs) {
        visitor.disableInstanceCheck();
        visitor.TraverseStmt(const_cast<clang::Stmt*>(ompDir.Node->getStructuredBlock()));
    }

    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<clang::Stmt, clang::OMPMasterDirective> ompMasterDirective;
    auto ompMasterMatcher = ompMasterDirective(hasStructuredBlock(stmt())).bind("OMPMaster");
    auto ompMasterDirs = getASTNodes<OMPMasterDirective>(Extractor.extract2(*Context, ompMasterMatcher), "OMPMaster");
    visitor.setInstanceName("#pragma omp master");
    for (auto ompDir : ompMasterDirs) {
        visitor.disableInstanceCheck();
        visitor.TraverseStmt(const_cast<clang::Stmt*>(ompDir.Node->getStructuredBlock()));
    }

    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<clang::Stmt, clang::OMPSingleDirective> ompSingleDirective;
    auto ompSingleMatcher = ompSingleDirective(hasStructuredBlock(stmt())).bind("OMPSingle");
    auto ompSingleDirs = getASTNodes<OMPSingleDirective>(Extractor.extract2(*Context, ompSingleMatcher), "OMPSingle");
    visitor.setInstanceName("#pragma omp single");
    for (auto ompDir : ompSingleDirs) {
        visitor.disableInstanceCheck();
        visitor.TraverseStmt(const_cast<clang::Stmt*>(ompDir.Node->getStructuredBlock()));
    }

    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<clang::Stmt, clang::OMPOrderedDirective> ompOrderedDirective;
    auto ompOrderedMatcher = ompOrderedDirective(hasStructuredBlock(stmt())).bind("OMPOrdered");
    auto ompOrderedDirs = getASTNodes<OMPOrderedDirective>(Extractor.extract2(*Context, ompOrderedMatcher), "OMPOrdered");
    visitor.setInstanceName("#pragma omp ordered");
    for (auto ompDir : ompOrderedDirs) {
        visitor.disableInstanceCheck();
        visitor.TraverseStmt(const_cast<clang::Stmt*>(ompDir.Node->getStructuredBlock()));
    }



    // pthread mutex lock
    auto pthreadMutexLockMatcher = callExpr(callee(functionDecl(hasName("pthread_mutex_lock"), isExpansionInFileMatching("pthread.h")))).bind("PthreadMutexLock");
    auto pthreadMutexLocks = getASTNodes<CallExpr>(Extractor.extract2(*Context, pthreadMutexLockMatcher), "PthreadMutexLock");

    std::vector<const clang::CompoundStmt*> enclosingStmts;
    for (auto match : pthreadMutexLocks) {
        unsigned line_num = Context->getSourceManager().getSpellingLineNumber(match.Node->getExprLoc());
        const clang::Stmt* currentStmt = llvm::dyn_cast<clang::Stmt>(Context->getParents(*match.Node).begin()->get<clang::Stmt>());

        while (currentStmt) {
            if (const clang::CompoundStmt* compoundStmt = llvm::dyn_cast<clang::CompoundStmt>(currentStmt)) {
                enclosingStmts.push_back(compoundStmt);
                break;
            }
            currentStmt = Context->getParents(*currentStmt).begin()->get<clang::Stmt>();
        }
    }

    visitor.setInstanceName("pthread_mutex_lock");
    for (auto stmt : enclosingStmts) {
        visitor.reset();
        visitor.TraverseStmt(const_cast<clang::CompoundStmt*>(stmt));
    }

    Features[scope_key_] = visitor.getFeatures();
}


void ScopeAnalysis::processFeatures(nlohmann::ordered_json j){
    nlohmann::ordered_json js(nlohmann::detail::value_t::object);
    for (auto& [instancename, twofeatures] : j.at(scope_key_).items()) {
        std::map<std::string, int> func_count;
        for (auto& func_call : twofeatures[RecursiveASTCollectVisitor::feature_function_]) {
            std::string function_name = func_call[RecursiveASTCollectVisitor::feature_function_name_key_].get<std::string>();
            std::string file_name = func_call[RecursiveASTCollectVisitor::feature_file_name_key_].get<std::string>();
            int line_num = func_call[RecursiveASTCollectVisitor::feature_line_num_key_].get<int>();
            if (deduplicator_.isDuplicated(file_name + std::to_string(line_num))) {
                continue;
            }
            func_count[function_name]++;
        }
        if (!func_count.empty()) {
            js[instancename][RecursiveASTCollectVisitor::feature_function_] = func_count;
        }

        std::map<std::string, std::map<std::string, int>> method_count;
        for (auto& [type_name, member_func_list] : twofeatures[RecursiveASTCollectVisitor::feature_member_method_].items()) {
            for (auto& member_func : member_func_list) {
                std::string method_name = member_func[RecursiveASTCollectVisitor::feature_member_method_name_key_].get<std::string>();
                std::string file_name = member_func[RecursiveASTCollectVisitor::feature_file_name_key_].get<std::string>();
                int line_num = member_func[RecursiveASTCollectVisitor::feature_line_num_key_].get<int>();
                if (deduplicator_.isDuplicated(file_name + std::to_string(line_num))) {
                    continue;
                }
                method_count[type_name][method_name]++;
            }
            
        }
        if (!method_count.empty()) {
            js[instancename][RecursiveASTCollectVisitor::feature_member_method_] = method_count;
        }
    }
    Statistics[scope_key_] = js;
}
