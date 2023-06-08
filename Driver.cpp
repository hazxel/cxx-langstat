// Sources for the ASTFrontendAction-ASTConsumer-MatchCallback "triad":
// Also has info on how to use FrontendActionFactory and ClangTool
// https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp
// and the corresponding conference video
// https://www.youtube.com/watch?v=E6i8jmiy8MY

// Information about ASTFrontendAction, ASTConsumer from official Clang doc.
// https://clang.llvm.org/docs/RAVFrontendAction.html

// Information about ASTFrontendAction, ASTContext and Clang AST in general:
// https://jonasdevlieghere.com/understanding-the-clang-ast/


// clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Path.h"
// standard includes
#include <iostream> // should be removed
#include <fstream> // file stream
// JSON library
#include <nlohmann/json.hpp>
//
#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/Utils.h"
#include "cxx-langstat/Driver.h"
#include "cxx-langstat/Stats.h"


using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using StringRef = llvm::StringRef;
using ASTContext = clang::ASTContext;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// Consumes the AST, i.e. does computations on it
class Consumer : public ASTConsumer {
public:
    Consumer(StringRef InFile, AnalysisRegistry* Registry) :
        InFile(InFile),
        Registry(Registry){
    }
    // Called when AST for TU is ready/has been parsed
    // Assumes -emit-features in on
    void HandleTranslationUnit(ASTContext& Context){
        ordered_json AllAnalysesFeatures;
        Registry->createFreshAnalyses();
        Stage Stage = Registry->Options.Stage;
        for(const auto& an : Registry->Analyses){ // ref to unique_ptr bad?
            auto AnalysisShorthand = an->getShorthand();
            // Analyze clang AST and get features
            AllAnalysesFeatures[AnalysisShorthand]
                =an->getFeatures(InFile, Context);
        }
        // Write to file if -emit-features is active
        auto OutputFile = Registry->getCurrentOutputFile();
        std::cout << "Writing features to file: "<<OutputFile<<"\n";
        std::ofstream o(OutputFile);
        o << AllAnalysesFeatures.dump(4) << '\n';
        Registry->destroyAnalyses();
    }
public:
    StringRef InFile;
    AnalysisRegistry* Registry;
};

// Responsible for steering when what is executed
class Action : public ASTFrontendAction {
public:
    Action(AnalysisRegistry* Registry) : Registry(Registry){
        // std::cout << "Creating AST Action" << std::endl;
    }
    // Called at start of processing a single input
    bool BeginSourceFileAction(CompilerInstance& CI) {
        std::cout
        << "Starting to process \033[32m" << getCurrentFile().str()
        << "\033[0m. AST=" << isCurrentFileAST() << ".\n";
        return true;
    }
    // Called after frontend is initialized, but before per-file processing
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, StringRef InFile){
            // std::cout << "Creating AST Consumer" << std::endl;
            return std::make_unique<Consumer>(getCurrentFile(), Registry);
    }
    //
    void EndSourceFileAction(){
        // std::cout << "Finished processing " << getCurrentFile().str() << ".\n";
    }
    AnalysisRegistry* Registry;
};

// Responsible for building Actions
class Factory : public clang::tooling::FrontendActionFactory {
public:
    Factory(AnalysisRegistry* Reg) : Registry(Reg){}
    //
    std::unique_ptr<FrontendAction> create() override {
        return std::make_unique<Action>(Registry);
    }
    AnalysisRegistry* Registry;
};

//-----------------------------------------------------------------------------
//
int CXXLangstatMain(std::vector<std::string> InputFiles,
    std::vector<std::string> OutputFiles, Stage Stage, std::string Analyses,
    std::string BuildPath, std::shared_ptr<CompilationDatabase> db){

    // Create custom options object for registry
    CXXLangstatOptions Opts(Stage, OutputFiles, Analyses);
    AnalysisRegistry* Registry = new AnalysisRegistry(Opts);

    if(Stage == emit_features){
        // https://clang.llvm.org/doxygen/CommonOptionsParser_8cpp_source.html @ 109
        // Read in database found in dir specified by -p or a parent path
        std::string ErrorMessage;
        if(!BuildPath.empty()){
            std::cout << "READING BUILD PATH\n";
            std::cout << "BUILD PATH: " << BuildPath << std::endl;
            db = CompilationDatabase::autoDetectFromDirectory(BuildPath, ErrorMessage);
            if(!ErrorMessage.empty()){
                std::cout << ErrorMessage << std::endl;
                exit(1);
            }
            std::cout << "FOUND COMPILE COMMANDS:" << std::endl;
            for(auto cc : db->getAllCompileCommands()){
                for(auto s : cc.CommandLine)
                    std::cout << s << " ";
                std::cout << std::endl;
            }
        }
        if(db){
            ClangTool Tool(*db, InputFiles);
            // Tool is run for every file specified in source path list
            Tool.run(std::make_unique<Factory>(Registry).get());
        } else {
            std::cout << "The tool couldn't run due to missing compilation database "
                "Please try one of the following:\n"
                " When analyzing .ast files, append \"--\", to indicate no CDB is necessary\n"
                " When analyzing .cpp files, append \"-- <compile flags>\" or \"-p <build path>\"\n";
        }
    }

    // Process features stored on disk to statistics
    else if(Stage == emit_statistics){
        ordered_json AllFilesAllStatistics;
        ordered_json Summary;
        for(auto File : InputFiles){
            std::cout << "Reading features from file: " << File << "...";
            ordered_json j;
            std::ifstream i(File);
            i >> j;
            std::cout << "Done\n";
            ordered_json OneFileAllStatistics;
            Registry->createFreshAnalyses();
            for(const auto& an : Registry->Analyses){ // ref to unique_ptr bad?
                auto AnalysisShorthand = an->getShorthand();
                for(const auto& [statdesc, stats] : an->getStatistics(j[AnalysisShorthand]).items()){
                    OneFileAllStatistics[statdesc] = stats;
                }
            }
            Summary = add(std::move(Summary), OneFileAllStatistics);
            AllFilesAllStatistics[File] = OneFileAllStatistics;
            Registry->destroyAnalyses();
        }
        std::ofstream o(Registry->Options.OutputFiles[0]);
        AllFilesAllStatistics["Summary"] = Summary;
        o << AllFilesAllStatistics.dump(4) << std::endl;
        //o << Summary.dump(4) << std::endl;
    }

    delete Registry;
    return 0;
}
