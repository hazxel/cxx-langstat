#include <iostream>

#include "llvm/Support/Commandline.h"
#include "clang/Tooling/CompilationDatabase.h"

#include "cxx-langstat/Options.h"
#include "../Driver.h"

#include <dirent.h>

using Twine = llvm::Twine;
using StringRef = llvm::StringRef;

using namespace clang::tooling;

//-----------------------------------------------------------------------------

// Global variables
// Options in CLI specific to cxx-langstat
llvm::cl::OptionCategory CXXLangstatCategory("cxx-langstat options", "");
llvm::cl::OptionCategory IOCategory("Input/output options", "");

// llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
// llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");
// CL options

// # probably going to retire soon
// Accepts comma-separated string of analyses
llvm::cl::opt<std::string> AnalysesOption(
    "analyses",
    llvm::cl::desc("Comma-separated list of analyses"),
    llvm::cl::cat(CXXLangstatCategory));

// 2 flags:
// --emit-features: analysis stops after writing features to file after reading in .ast
// --emit-statistics: read in JSON with features and compute statistics
// no flag at all: compute features but don't write them to disk, compute
// statistics and emit them
llvm::cl::opt<Stage> PipelineStage(
    llvm::cl::desc("Stage: "),
    llvm::cl::values(
        clEnumValN(emit_features, "emit-features",
            "Stop after emitting features.\n"
            "If output files are specified, \n "
            "on output for every input file \n"
            "has to be specified. If no output \n"
            "is specified, output files will \n"
            "will be put at the working directory"),
        clEnumValN(emit_statistics, "emit-statistics", "Read in .json files "
            "each containing features extracted from an AST and compute "
            "statistics. Requires either no output files or only a single one")),
    llvm::cl::cat(CXXLangstatCategory));

// --in option, all until --out is taken as input files
llvm::cl::list<std::string> InputFilesOption(
    "in",
    llvm::cl::desc("<ast0> [... <astN>]"),
    llvm::cl::Positional,
    llvm::cl::ValueRequired,
    llvm::cl::ZeroOrMore,
    llvm::cl::cat(IOCategory));

// --out option, optional. when used, should give same #args as with --in.
llvm::cl::opt<std::string> OutputFileOption(
    "out",
    llvm::cl::desc("[<json1> ... <jsonN>]"),
    llvm::cl::ValueRequired,
    llvm::cl::cat(IOCategory));

llvm::cl::opt<std::string> InputDirOption(
    "indir",
    llvm::cl::desc("<dir>"),
    llvm::cl::ValueRequired,
    llvm::cl::cat(IOCategory));

llvm::cl::opt<std::string> OutputDirOption(
    "outdir",
    llvm::cl::desc("<dir>"),
    llvm::cl::ValueRequired,
    llvm::cl::cat(IOCategory));

// what to do with this? some -p option already there by default, but parser fails on it
static llvm::cl::opt<std::string> BuildPath("p", llvm::cl::desc("Build path, but really"),
   llvm::cl::Optional, llvm::cl::cat(CXXLangstatCategory));

//-----------------------------------------------------------------------------
bool hasSuitableExtension(llvm::StringRef s, Stage Stage){
    if(Stage==emit_features) {
        return s.equals(".cpp") || s.equals(".cc") || s.equals(".cxx") || s.equals(".C")
            || s.equals(".hpp") || s.equals(".hh") || s.equals(".hxx") || s.equals(".H")
            || s.equals(".c++") || s.equals(".h++")
            || s.equals(".c") || s.equals(".h") // C file formats
            || s.equals(".ast"); // AST file
    } else if(Stage == emit_statistics){
        return s.equals(".json");
    }
    return false;
}

std::vector<std::string> getFiles(const Twine& T, Stage Stage){
    // http://www.martinbroadhurst.com/list-the-files-in-a-directory-in-c.html
    // Trust me, would prefer to use <filesystem> too, but I'd have to upgrade
    // to macOS 10.15. Might changed this to use conditional compilation
    // to enable <filesystem> for OSes that can use it.
    DIR* dirp = opendir(T.str().c_str());
    if(dirp){
        // std::cout << "dir: " << T.str().c_str() << std::endl;
        struct dirent* dp;
        std::vector<std::string> files;
        std::vector<std::string> res;
        while ((dp = readdir(dirp)) != NULL) {
            files.emplace_back(dp->d_name);
        }
        closedir(dirp);
        std::vector<std::string> dirfiles;
        for(auto file : files){
            if(!llvm::StringRef(file).consume_front(".")){ //ignore hidden files & dirs
                if(hasSuitableExtension(llvm::sys::path::extension(file), Stage)){
                    res.emplace_back((T + "/" + file).str());
                } else if(!llvm::sys::path::filename(file).equals(".")
                    && !llvm::sys::path::filename(file).equals("..")) {
                        dirfiles.emplace_back((T + "/" + file).str());
                }
            }
        }
        for(auto dirfile : dirfiles) {
            auto files = getFiles(dirfile, Stage);
            for(auto file : files)
                res.emplace_back(file);
        }
        return res;
    }
    return {};
}

//-----------------------------------------------------------------------------

//
int main(int argc, char** argv){
    // Common parser for command line options, provided by llvm
    // CommonOptionsParser Parser(argc, argv, CXXLangstatCategory);
    // const std::vector<std::string>& spl = Parser.getSourcePathList();
    // CompilationDatabase& db = Parser.getCompilations();
    // I don't like the way input/source files are handled by COP, so I roll
    // my own stuff.
    std::unique_ptr<CompilationDatabase> db = nullptr;
    // First try to get string of cxxflags after '--', as loadFromCommandLine requires
    std::string ErrorMessage;
    db = FixedCompilationDatabase::loadFromCommandLine(argc, argv, ErrorMessage);
    if(db) {
        std::cout << "COMPILE COMMAND: ";
        for(auto cc : db->getCompileCommands("")){
            for(auto s : cc.CommandLine)
                std::cout << s << " ";
            std::cout << std::endl;
        }
    } else {
        std::cout << "Could not load compile command from command line \n" +
            ErrorMessage;
    }

    // Only now can call this method, otherwise compile command could be interpreted
    // as input or output file since those are positional
    // This usage is encouraged this way according to
    // https://clang.llvm.org/doxygen/classclang_1_1tooling_1_1FixedCompilationDatabase.html#a1443b7812e6ffb5ea499c0e880de75fc
    llvm::cl::ParseCommandLineOptions(argc, argv, "cxx-langstat is a clang-based"
     "tool for computing statistics on C/C++ code on the clang AST level");

    std::vector<std::string> InputFiles;
    std::vector<std::string> OutputFiles;

    // const std::vector<int> a; // no init?

    bool Files = !InputFilesOption.empty();
    bool Dir = !InputDirOption.empty();
    if(Files && Dir){
        std::cout << "Don't specify both input files and directory "
            "at the same time\n";
        exit(1);
    }

    if(Files){
        InputFiles = InputFilesOption;
    } else {
        InputFiles = getFiles(InputDirOption, PipelineStage);
    }


    // When multiple output files are a fact (multiple input files) or very
    // likely (input dir), require OutputDirOption instead of OutputFileOption to be used.
    // OutputFiles can only be used when it is guaranteed to be only a single output file.
    // emit-features creates one output per input.
    if(PipelineStage == emit_features){
        if(Files){
            if(InputFiles.size() == 1){ // single file
                if(!OutputDirOption.empty()){ // may not specify output dir
                    exit(1);
                }
                if(!OutputFileOption.empty()){ // place at output file specified
                    OutputFiles.emplace_back("./" + OutputFileOption);
                } else { // create output file if none specified
                    StringRef filename = llvm::sys::path::filename(InputFiles[0]);
                    filename.consume_back(llvm::sys::path::extension(filename));
                    OutputFiles.emplace_back("./" + filename.str() + ".features.json");

                }
            } else { // multiple files
                if(!OutputFileOption.empty()){ // may not specify output file
                    exit(1);
                }
                if(OutputDirOption.empty()){ // obliged to specify output dir
                    exit(1);
                } else {
                    for(auto File : InputFiles){ // place at output dir specified
                        StringRef filename = llvm::sys::path::filename(File);
                        filename.consume_back(llvm::sys::path::extension(filename));
                        OutputFiles.emplace_back(OutputDirOption + "/" + filename.str() + ".features.json");
                    }
                }
            }
        }
        if(Dir){ // same behavior as with multiple files specified
            if(!OutputFileOption.empty())
                exit(1);
            if(OutputDirOption.empty()){
                exit(1);
            } else { // place at output dir specified
                for(const auto& InputFile : InputFiles){
                    StringRef filename = llvm::sys::path::filename(InputFile);
                    filename.consume_back(llvm::sys::path::extension(filename));
                    OutputFiles.emplace_back(OutputDirOption + "/" + filename.str() + ".features.json");
                }
            }
        }
        assert(InputFiles.size() == OutputFiles.size());
    // When -emit-features option is not used, only zero or one output file is ok.
    // Output dir is not ok, since the output is guaranteed to be only a single file.
    } else {
        if(!OutputDirOption.empty())
            exit(1);
        if(OutputFileOption.empty()){
            OutputFiles.emplace_back("./stats.json");
        }
        if(!OutputFileOption.empty())
            OutputFiles.emplace_back(OutputFileOption);
        assert(OutputFiles.size() == 1);
    }

    std::cout << "input files(" << InputFiles.size() << "): ";
    for(const auto& InputFile : InputFiles){
        std::cout << InputFile << " ";
        if(StringRef(InputFile).consume_back("/")){
            std::cout << "Specified input dir, quitting.. \n";
            exit(1);
        }
    }
    std::cout << '\n';
    std::cout << "output files(" << OutputFiles.size() << "): ";
    for(const auto& OutputFile : OutputFiles){
        std::cout << OutputFile << " ";
        if(StringRef(OutputFile).consume_back("/")){
            std::cout << "Specified output dir, quitting.. \n";
            exit(1);
        }
    }

    CXXLangstatMain(InputFiles, OutputFiles,
        PipelineStage, AnalysesOption, BuildPath, std::move(db));
    return 0;
}
