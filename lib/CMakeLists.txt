add_library(MatchingExtractor MatchingExtractor.cpp)
add_library(AnalysisList AnalysisList.cpp)
add_library(AnalysisRegistry AnalysisRegistry.cpp)
add_library(Stats Stats.cpp)
add_library(Utils Utils.cpp)
add_library(RecursiveASTCollectVisitor RecursiveASTCollectVisitor.cpp)
add_library(Deduplicator Deduplicator.cpp)

add_subdirectory(Analyses)
