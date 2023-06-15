
#ifndef OMPEXECUTABLEDIRECTIVEANALYSIS_H
#define OMPEXECUTABLEDIRECTIVEANALYSIS_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Deduplicator.h"


//-----------------------------------------------------------------------------

class OMPExecutableDirectiveAnalysis : public Analysis {
public:
    OMPExecutableDirectiveAnalysis();
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;

    static Deduplicator deduplicator_;
    static constexpr auto ShorthandName = "ompeda";

protected:
    const std::string omp_directive_file_name_key_ = "file_name";
    const std::string omp_directive_begin_line_num_key_ = "begin_line_num";
    const std::string omp_directive_end_line_num_key_ = "end_line_num";
    const std::string omp_directive_key_ = "omp_directive";
    // const std::string omp_total_directives_key_ = "omp_total";
    // const std::string omp_parallel_directive_key_ = "omp_parallel";
    // const std::string omp_parallel_for_directive_key_ = "omp_parallel_for";
    // const std::string omp_critical_directive_key_ = "omp_critical";
    // const std::string omp_barrier_directive_key_ = "omp_barrier";
    // const std::string omp_atomic_directive_key_ = "omp_atomic";
    // const std::string omp_ordered_directive_key_ = "omp_ordered";
    // const std::string omp_loop_directive_key_ = "omp_loop";
};

//-----------------------------------------------------------------------------

#endif // OMPEXECUTABLEDIRECTIVEANALYSIS_H
