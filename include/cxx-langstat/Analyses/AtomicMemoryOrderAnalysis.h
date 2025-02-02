
#ifndef ATOMICMEMORYORDERANALYSIS_H
#define ATOMICMEMORYORDERANALYSIS_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Deduplicator.h"

//-----------------------------------------------------------------------------

class AtomicMemoryOrderAnalysis : public Analysis {
public:
    AtomicMemoryOrderAnalysis();
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;

    static Deduplicator deduplicator_;
    Matches<clang::CXXMemberCallExpr> atomic_calls_;
    Matches<clang::CXXMemberCallExpr> typedef_atomic_calls_;
    Matches<clang::CallExpr> atomic_function_calls_;
    static constexpr auto ShorthandName = "amoa";

protected:
    const clang::ast_matchers::internal::Matcher<clang::NamedDecl> atomic_instance_names_ = clang::ast_matchers::hasAnyName("std::atomic", "std::atomic_int", "std::atomic_uint", "std::atomic_long", "std::atomic_ulong", "std::atomic_llong", "std::atomic_ullong", "std::atomic_char", "std::atomic_schar", "std::atomic_uchar", "std::atomic_short", "std::atomic_ushort", "std::atomic_wchar_t", "std::atomic_char16_t", "std::atomic_char32_t", "std::atomic_bool", "std::atomic_float", "std::atomic_double", "std::atomic_long_double", "std::atomic_size_t", "std::atomic_ptrdiff_t", "std::atomic_int_least8_t", "std::atomic_uint_least8_t", "std::atomic_int_least16_t", "std::atomic_uint_least16_t", "std::atomic_int_least32_t", "std::atomic_uint_least32_t", "std::atomic_int_least64_t", "std::atomic_uint_least64_t", "std::atomic_int_fast8_t", "std::atomic_uint_fast8_t", "std::atomic_int_fast16_t", "std::atomic_uint_fast16_t", "std::atomic_int_fast32_t", "std::atomic_uint_fast32_t", "std::atomic_int_fast64_t", "std::atomic_uint_fast64_t", "std::atomic_intptr_t", "std::atomic_uintptr_t", "std::atomic_intmax_t", "std::atomic_uintmax_t");
    const clang::ast_matchers::internal::Matcher<clang::NamedDecl> atomic_func_names_ = clang::ast_matchers::hasAnyName("std::atomic_store", "std::atomic_store_explicit", "std::atomic_load", "std::atomic_load_explicit", "std::atomic_exchange", "std::atomic_exchange_explicit", "std::atomic_compare_exchange_weak", "std::atomic_compare_exchange_strong", "std::atomic_compare_exchange_weak_explicit", "std::atomic_compare_exchange_strong_explicit", "std::atomic_fetch_add", "std::atomic_fetch_add_explicit", "std::atomic_fetch_sub", "std::atomic_fetch_sub_explicit", "std::atomic_fetch_and", "std::atomic_fetch_and_explicit", "std::atomic_fetch_or", "std::atomic_fetch_or_explicit", "std::atomic_fetch_xor", "std::atomic_fetch_xor_explicit", "std::atomic_wait", "std::atomic_wait_explicit", "std::atomic_notify_one", "std::atomic_notify_all");
    const std::string atomic_header_file_names_ = "atomic|std/atomic";
    const std::string memory_order_key_ = "memory_order";
    const std::string memory_order_list_key_ = "memory_order_list";
    const std::string line_number_key_ = "line_number";
    const std::string file_name_key_ = "file_name";
};

//-----------------------------------------------------------------------------

#endif // ATOMICMEMORYORDERANALYSIS_H
