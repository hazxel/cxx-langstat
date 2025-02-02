#include <iostream>

#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/AnalysisList.h"

#include "cxx-langstat/Analyses/AlgorithmLibraryAnalysis.h"
#include "cxx-langstat/Analyses/AtomicMemoryOrderAnalysis.h"
#include "cxx-langstat/Analyses/ClassInstantiationAnalysis.h"
#include "cxx-langstat/Analyses/ClassMethodCallAnalysis.h"
#include "cxx-langstat/Analyses/ClassMethodCallTranslationUnitAnalysis.h"
#include "cxx-langstat/Analyses/ConcurrencySupportLibraryAnalysis.h"
#include "cxx-langstat/Analyses/ConcurrencySupportLibraryMethodAnalysis.h"
#include "cxx-langstat/Analyses/ConcurrencySupportLibraryMethodTranslationUnitAnalysis.h"
#include "cxx-langstat/Analyses/ConstexprAnalysis.h"
#include "cxx-langstat/Analyses/CyclomaticComplexityAnalysis.h"
#include "cxx-langstat/Analyses/ContainerLibAnalysis.h"
#include "cxx-langstat/Analyses/DataContainerAnalysis.h"
#include "cxx-langstat/Analyses/DataContainerTranslationUnitAnalysis.h"
#include "cxx-langstat/Analyses/OMPExecutableDirectiveTranslationUnitAnalysis.h"
#include "cxx-langstat/Analyses/STLMutexScopeAnalysis.h"
#include "cxx-langstat/Analyses/ScopeAnalysis.h"
#include "cxx-langstat/Analyses/FunctionParameterAnalysis.h"
#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"
#include "cxx-langstat/Analyses/LoopKindAnalysis.h"
#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"
#include "cxx-langstat/Analyses/OMPExecutableDirectiveAnalysis.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Analyses/TemplateParameterAnalysis.h"
#include "cxx-langstat/Analyses/UsingAnalysis.h"
#include "cxx-langstat/Analyses/UtilityLibAnalysis.h"
#include "cxx-langstat/Analyses/VariableTemplateAnalysis.h"

//-----------------------------------------------------------------------------

AnalysisRegistry::AnalysisRegistry(CXXLangstatOptions Opts) : Options(Opts) {
    std::cout << "Registry ctor" << std::endl;
}
AnalysisRegistry::~AnalysisRegistry(){
    std::cout << "Registry dtor" << std::endl;
}

// Register new analyses here.
void AnalysisRegistry::createFreshAnalyses(){
    std::cout << "Creating analyses" << std::endl;
    // Example of where std::move could be used if std::make<"Analysis">
    // was defined explicitly as a variable
    // Create all analyses and give them to the registry

    // You don't have to add analyses in a particular order, but I think
    // that alphabetically is the nicest, because it will affect in what order
    // features and statistics will be printed at the end.

    if(Options.EnabledAnalyses.contains("ala"))
        Analyses.emplace_back(std::make_unique<AlgorithmLibraryAnalysis>());
    if(Options.EnabledAnalyses.contains("amoa"))
        Analyses.emplace_back(std::make_unique<AtomicMemoryOrderAnalysis>());
    if(Options.EnabledAnalyses.contains("cca"))
        Analyses.emplace_back(std::make_unique<CyclomaticComplexityAnalysis>());
    if(Options.EnabledAnalyses.contains("cea"))
        Analyses.emplace_back(std::make_unique<ConstexprAnalysis>());
    if(Options.EnabledAnalyses.contains("cia"))
        Analyses.emplace_back(std::make_unique<ClassInstantiationAnalysis>());
    if(Options.EnabledAnalyses.contains("cla"))
        Analyses.emplace_back(std::make_unique<ContainerLibAnalysis>());
    if(Options.EnabledAnalyses.contains("cmca"))
        Analyses.emplace_back(std::make_unique<ClassMethodCallAnalysis>());
    if(Options.EnabledAnalyses.contains("cmcatu"))
        Analyses.emplace_back(std::make_unique<ClassMethodCallTranslationUnitAnalysis>());
    if(Options.EnabledAnalyses.contains("csla"))
        Analyses.emplace_back(std::make_unique<ConcurrencySupportLibraryAnalysis>());
    if(Options.EnabledAnalyses.contains("cslma"))
        Analyses.emplace_back(std::make_unique<ConcurrencySupportLibraryMethodAnalysis>());
    if(Options.EnabledAnalyses.contains("cslmatu"))
        Analyses.emplace_back(std::make_unique<ConcurrencySupportLibraryMethodTranslationUnitAnalysis>());
    if(Options.EnabledAnalyses.contains("dca"))
        Analyses.emplace_back(std::make_unique<DataContainerAnalysis>());
    if(Options.EnabledAnalyses.contains("dcatu"))
        Analyses.emplace_back(std::make_unique<DataContainerTranslationUnitAnalysis>());
    if(Options.EnabledAnalyses.contains("ompedatu"))
        Analyses.emplace_back(std::make_unique<OMPExecutableDirectiveTranslationUnitAnalysis>());
    if(Options.EnabledAnalyses.contains("sa"))
        Analyses.emplace_back(std::make_unique<ScopeAnalysis>());
    if(Options.EnabledAnalyses.contains("fpa"))
        Analyses.emplace_back(std::make_unique<FunctionParameterAnalysis>());
    if(Options.EnabledAnalyses.contains("lda"))
        Analyses.emplace_back(std::make_unique<LoopDepthAnalysis>(5));
    if(Options.EnabledAnalyses.contains("lka"))
        Analyses.emplace_back(std::make_unique<LoopKindAnalysis>());
    if(Options.EnabledAnalyses.contains("msa"))
        Analyses.emplace_back(std::make_unique<msa::MoveSemanticsAnalysis>());
    if(Options.EnabledAnalyses.contains("ompeda"))
        Analyses.emplace_back(std::make_unique<OMPExecutableDirectiveAnalysis>());
    if(Options.EnabledAnalyses.contains("smsa"))
        Analyses.emplace_back(std::make_unique<STLMutexScopeAnalysis>());
    if(Options.EnabledAnalyses.contains("tia"))
        Analyses.emplace_back(std::make_unique<TemplateInstantiationAnalysis>());
    if(Options.EnabledAnalyses.contains("tpa"))
        Analyses.emplace_back(std::make_unique<TemplateParameterAnalysis>());
    if(Options.EnabledAnalyses.contains("ua"))
        Analyses.emplace_back(std::make_unique<UsingAnalysis>());
    if(Options.EnabledAnalyses.contains("ula"))
        Analyses.emplace_back(std::make_unique<UtilityLibAnalysis>());
    if(Options.EnabledAnalyses.contains("vta"))
        Analyses.emplace_back(std::make_unique<VariableTemplateAnalysis>());
}

//-----------------------------------------------------------------------------
