
#include "cxx-langstat/Analyses/AtomicMemoryOrderAnalysis.h"
#include "cxx-langstat/Utils.h"

#include <unordered_map>

using std::string;
using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

auto AtomicMemoryOrderAnalysis::deduplicator_ = Deduplicator();

AtomicMemoryOrderAnalysis::AtomicMemoryOrderAnalysis() {}

//-----------------------------------------------------------------------------
// FIXME: implementation
void AtomicMemoryOrderAnalysis::analyzeFeatures(){
    auto atomic_matcher = cxxMemberCallExpr(on(hasType(cxxRecordDecl(atomic_instance_names_, isExpansionInFileMatching(atomic_header_file_names_))))).bind("AtomicCall");
    atomic_calls_ = getASTNodes<CXXMemberCallExpr>(Extractor.extract2(*Context, atomic_matcher), "AtomicCall");
    
    auto typedef_atomic_matcher = cxxMemberCallExpr(on(hasType(typedefDecl(atomic_instance_names_, isExpansionInFileMatching(atomic_header_file_names_))))).bind("TypedefAtomic");
    typedef_atomic_calls_ = getASTNodes<CXXMemberCallExpr>(Extractor.extract2(*Context, typedef_atomic_matcher), "TypedefAtomic");
    
    atomic_calls_.insert(atomic_calls_.end(), typedef_atomic_calls_.begin(), typedef_atomic_calls_.end());
    
    // auto atomic_operator_matcher = cxxOperatorCallExpr(on(hasType(cxxRecordDecl(atomic_instance_names_, isExpansionInFileMatching(atomic_header_file_names_))))).bind("AtomicOperatorCall");
    // auto atomic_operator_calls_ = getASTNodes<CXXOperatorCallExpr>(Extractor.extract2(*Context, atomic_operator_matcher), "AtomicOperatorCall");

    ordered_json js;

    for (auto match : atomic_calls_) {
        string called_func = match.Node->getMethodDecl()->getQualifiedNameAsString();
        string memory_model = "";
        bool has_memory_order_arg = false;

        unsigned num_args = match.Node->getNumArgs();

        std::string printedStmt;
        llvm::raw_string_ostream stream(printedStmt);

        for (int i = 0; i < num_args; i++) {
            auto model = match.Node->getArg(i)->getExprStmt();
            if (model->getType().getAsString().find("memory_order") != std::string::npos) {
                has_memory_order_arg = true;
                model->printPretty(stream, nullptr, PrintingPolicy(LangOptions()));
            } else {
                continue;
            }
        }

        if (!has_memory_order_arg) {
            continue;
        }

        string expr = stream.str();
        if (expr.length() == 0) {
            memory_model = "std::memory_order_seq_cst";
        } else {
            memory_model = expr;
        }        
        

        unsigned line_num = Context->getSourceManager().getSpellingLineNumber(match.Node->getExprLoc());
        auto persumed_loc = Context->getSourceManager().getPresumedLoc(match.Node->getExprLoc());
        const char* file_name = persumed_loc.getFilename();

        ordered_json j;
        j[line_number_key_] = line_num;
        j[file_name_key_] = file_name;
        j[called_func_key_] = called_func;
        j[is_default_key_] = expr.length() == 0;
        js[memory_model].emplace_back(j);
    }

    Features[memory_model_key_] = js;

}
// FIXME: implementation
void AtomicMemoryOrderAnalysis::processFeatures(nlohmann::ordered_json j){
    std::unordered_map<string, unsigned> memory_model_count;
    for (auto& [memory_model, calls] : j.at(memory_model_key_).items()) {
        for (auto& call : calls) {
            string file_name = call[file_name_key_].get<string>();
            unsigned line_num = call[line_number_key_].get<int>();
            string function_name = call[called_func_key_].get<string>();
            bool is_default = call[is_default_key_].get<bool>();

            string key = file_name + std::to_string(line_num) + function_name;
            if (isDependentHeader(file_name)
                || deduplicator_.isDuplicated(key)) {
                continue;
            }

            memory_model_count[is_default ? "std::memory_order_seq_cst(default)" : memory_model]++;

        }
    }
    Statistics[memory_model_key_] = memory_model_count;
}

//-----------------------------------------------------------------------------

