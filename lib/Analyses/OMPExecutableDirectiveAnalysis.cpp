
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

    ordered_json js;

    for (auto match : ompExecDirs) {
        std::string dirName = match.Node->getStmtClassName();
        unsigned beginLineNum = Context->getSourceManager().getSpellingLineNumber(match.Node->getBeginLoc());
        unsigned endLineNum = Context->getSourceManager().getSpellingLineNumber(match.Node->getEndLoc());
        std::string fileName = Context->getSourceManager().getFilename(match.Node->getBeginLoc()).str();

        auto atomicDir = dyn_cast<OMPAtomicDirective>(match.Node);
        if (atomicDir) {
            unsigned numofClauses = atomicDir->getNumClauses();
            for (int i = 0; i < numofClauses; ++i) {
                OMPClause *clause = atomicDir->getClause(i);
                if (dyn_cast<OMPUpdateClause>(clause)) {
                    dirName += "Update";
                } else if (dyn_cast<OMPCaptureClause>(clause)) {
                    dirName += "Capture";
                } else if (dyn_cast<OMPReadClause>(clause)) {
                    dirName += "Read";
                } else if (dyn_cast<OMPWriteClause>(clause)) {
                    dirName += "Write";
                } else if (dyn_cast<OMPSeqCstClause>(clause)) {
                    dirName += "SeqCst";
                } else if (dyn_cast<OMPAcqRelClause>(clause)) {
                    dirName += "AcqRel";
                } else if (dyn_cast<OMPReleaseClause>(clause)) {
                    dirName += "Release";
                } else if (dyn_cast<OMPAcquireClause>(clause)) {
                    dirName += "Acquire";
                } else if (dyn_cast<OMPRelaxedClause>(clause)) {
                    dirName += "Relaxed";
                }
            }
        }

        if (isDependentHeader(fileName)) {
            continue;
        }

        ordered_json j;
        j[omp_directive_begin_line_num_key_] = beginLineNum;
        j[omp_directive_end_line_num_key_] = endLineNum;
        j[omp_directive_file_name_key_] = fileName;
        js[dirName].emplace_back(j);
    }

    Features[omp_directive_key_] = js;
}

void OMPExecutableDirectiveAnalysis::processFeatures(nlohmann::ordered_json j){
    std::unordered_map<std::string, unsigned> ompDirectives;
    for (auto& [dirName, dirList] : j.at(omp_directive_key_).items()) {
        for (auto& dir : dirList) {
            if (deduplicator_.isDuplicated(
                dirName + dir[omp_directive_file_name_key_].get<string>() 
                + std::to_string(dir[omp_directive_begin_line_num_key_].get<int>()) 
                + "-" + std::to_string(dir[omp_directive_end_line_num_key_].get<int>()))
            ) {
                continue;
            }
            ompDirectives[dirName]++;
        }
    }
    Statistics[omp_directive_key_] = ompDirectives;
}

//-----------------------------------------------------------------------------

