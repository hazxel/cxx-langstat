
#include "cxx-langstat/Analyses/OMPExecutableDirectiveAnalysis.h"
#include "cxx-langstat/Utils.h"


using namespace clang;
using namespace clang::ast_matchers;
using std::string;
using ordered_json = nlohmann::ordered_json;


auto OMPExecutableDirectiveAnalysis::deduplicator_ = Deduplicator();


// FIXME:
// Use nontrivial destructor if necessary
OMPExecutableDirectiveAnalysis::OMPExecutableDirectiveAnalysis() {}

//-----------------------------------------------------------------------------
// FIXME: implementation
void OMPExecutableDirectiveAnalysis::analyzeFeatures(){
    auto ompExecDirMatcher = ompExecutableDirective().bind("OMPExecutableDirective");
    auto ompExecDirs = getASTNodes<OMPExecutableDirective>(Extractor.extract2(*Context, ompExecDirMatcher), "OMPExecutableDirective");
    
    // // #pragma omp parallel
    // const internal::VariadicDynCastAllOfMatcher<Stmt, OMPParallelDirective> ompParallelDirective;
    // auto ompParallelDirMatcher = ompParallelDirective().bind("OMPParallelDirective");
    // auto ompParallelDirs = getASTNodes<OMPParallelDirective>(Extractor.extract2(*Context, ompParallelDirMatcher), "OMPParallelDirective");

    // // #pragma omp parallel for
    // const internal::VariadicDynCastAllOfMatcher<Stmt, OMPParallelForDirective> ompParallelForDirective;
    // auto ompParallelForDirMatcher = ompParallelForDirective().bind("OMPParallelForDirective");
    // auto ompParallelForDirs = getASTNodes<OMPParallelForDirective>(Extractor.extract2(*Context, ompParallelForDirMatcher), "OMPParallelForDirective");

    // // #pragma omp critical
    // const internal::VariadicDynCastAllOfMatcher<Stmt, OMPCriticalDirective> ompCriticalDirective;
    // auto ompCriticalDirMatcher = ompCriticalDirective().bind("OMPCriticalDirective");
    // auto ompCriticalDirs = getASTNodes<OMPCriticalDirective>(Extractor.extract2(*Context, ompCriticalDirMatcher), "OMPCriticalDirective");

    // // #pragma omp barrier
    // const internal::VariadicDynCastAllOfMatcher<Stmt, OMPBarrierDirective> ompBarrierDirective;
    // auto ompBarrierDirMatcher = ompBarrierDirective().bind("OMPBarrierDirective");
    // auto ompBarrierDirs = getASTNodes<OMPBarrierDirective>(Extractor.extract2(*Context, ompBarrierDirMatcher), "OMPBarrierDirective");

    // // #pragma omp atomic
    // const internal::VariadicDynCastAllOfMatcher<Stmt, OMPAtomicDirective> ompAtomicDirective;
    // auto ompAtomicDirMatcher = ompAtomicDirective().bind("OMPAtomicDirective");
    // auto ompAtomicDirs = getASTNodes<OMPAtomicDirective>(Extractor.extract2(*Context, ompAtomicDirMatcher), "OMPAtomicDirective");

    // // #pragma omp ordered
    // const internal::VariadicDynCastAllOfMatcher<Stmt, OMPOrderedDirective> ompOrderedDirective;
    // auto ompOrderedDirMatcher = ompOrderedDirective().bind("OMPOrderedDirective");
    // auto ompOrderedDirs = getASTNodes<OMPOrderedDirective>(Extractor.extract2(*Context, ompOrderedDirMatcher), "OMPOrderedDirective");

    // // #pragma omp loop
    // const internal::VariadicDynCastAllOfMatcher<Stmt, OMPLoopDirective> ompLoopDirective;
    // auto ompLoopDirMatcher = ompLoopDirective().bind("OMPLoopDirective");
    // auto ompLoopDirs = getASTNodes<OMPLoopDirective>(Extractor.extract2(*Context, ompLoopDirMatcher), "OMPLoopDirective");


    ordered_json js;

    for (auto match : ompExecDirs) {
        std::string dirName = match.Node->getStmtClassName();
        unsigned beginLineNum = Context->getSourceManager().getSpellingLineNumber(match.Node->getBeginLoc());
        unsigned endLineNum = Context->getSourceManager().getSpellingLineNumber(match.Node->getEndLoc());
        std::string fileName = Context->getSourceManager().getFilename(match.Node->getBeginLoc()).str();

        ordered_json j;
        j[omp_directive_begin_line_num_key_] = beginLineNum;
        j[omp_directive_end_line_num_key_] = endLineNum;
        j[omp_directive_file_name_key_] = fileName;
        js[dirName].emplace_back(j);
    }

    // js[omp_total_directives_key_] = ompExecDirs.size();
    // js[omp_parallel_directive_key_] = ompParallelDirs.size();
    // js[omp_parallel_for_directive_key_] = ompParallelForDirs.size();
    // js[omp_critical_directive_key_] = ompCriticalDirs.size();
    // js[omp_barrier_directive_key_] = ompBarrierDirs.size();
    // js[omp_atomic_directive_key_] = ompAtomicDirs.size();
    // js[omp_ordered_directive_key_] = ompOrderedDirs.size();
    // js[omp_loop_directive_key_] = ompLoopDirs.size();

    Features[omp_directive_key_] = js;
}

void OMPExecutableDirectiveAnalysis::processFeatures(nlohmann::ordered_json j){
    std::unordered_map<std::string, unsigned> ompDirectives;
    for (auto& [dirName, dirList] : j.at(omp_directive_key_).items()) {
        for (auto& dir : dirList) {
            if (isDependentHeader(dir[omp_directive_file_name_key_].get<string>()) 
                || deduplicator_.isDuplicated(dirName + dir[omp_directive_file_name_key_].get<string>() + std::to_string(dir[omp_directive_begin_line_num_key_].get<int>()) + "-" + std::to_string(dir[omp_directive_end_line_num_key_].get<int>()))) {
                continue;
            }
            ompDirectives[dirName]++;
        }
    }
    Statistics[omp_directive_key_] = ompDirectives;
}

//-----------------------------------------------------------------------------

