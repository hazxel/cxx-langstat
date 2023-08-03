#include "cxx-langstat/Analyses/ConcurrencySupportLibraryMethodAnalysis.h"
#include "cxx-langstat/Utils.h"

ConcurrencySupportLibraryMethodAnalysis::ConcurrencySupportLibraryMethodAnalysis() : ClassMethodCallAnalysis(
        clang::ast_matchers::hasAnyName(
        // standard library concurrency supports
        "std::thread", "std::jthread", // threads
        "std::atomic", "std::atomic_int", "std::atomic_uint", "std::atomic_bool", "std::atomic_char", "std::atomic_schar", "std::atomic_uchar", "std::atomic_short", "std::atomic_ushort", "std::atomic_wchar_t", "std::atomic_char16_t", "std::atomic_char32_t", "std::atomic_int8_t", "std::atomic_uint8_t", "std::atomic_int16_t", "std::atomic_uint16_t", "std::atomic_int32_t", "std::atomic_uint32_t", "std::atomic_int64_t", "std::atomic_uint64_t", "std::atomic_int_least8_t", "std::atomic_uint_least8_t", "std::atomic_int_least16_t", "std::atomic_uint_least16_t", "std::atomic_int_least32_t", "std::atomic_uint_least32_t", "std::atomic_int_least64_t", "std::atomic_uint_least64_t", "std::atomic_int_fast8_t", "std::atomic_uint_fast8_t", "std::atomic_int_fast16_t", "std::atomic_uint_fast16_t", "std::atomic_int_fast32_t", "std::atomic_uint_fast32_t", "std::atomic_int_fast64_t", "std::atomic_uint_fast64_t", "std::atomic_intptr_t", "std::atomic_uintptr_t", "std::atomic_intmax_t", "std::atomic_uintmax_t",  // atomic operations
        "std::atomic_store", "std::atomic_store_explicit", "std::atomic_load", "std::atomic_load_explicit", "std::atomic_exchange", "std::atomic_exchange_explicit", "std::atomic_compare_exchange_weak", "std::atomic_compare_exchange_strong", "std::atomic_compare_exchange_weak_explicit", "std::atomic_compare_exchange_strong_explicit", "std::atomic_fetch_add", "std::atomic_fetch_add_explicit", "std::atomic_fetch_sub", "std::atomic_fetch_sub_explicit", "std::atomic_fetch_and", "std::atomic_fetch_and_explicit", "std::atomic_fetch_or", "std::atomic_fetch_or_explicit", "std::atomic_fetch_xor", "std::atomic_fetch_xor_explicit", "std::atomic_wait", "std::atomic_wait_explicit", "std::atomic_notify_one", "std::atomic_notify_all", // atomic operations
        "std::mutex", "std::timed_mutex", "std::recursive_mutex", "std::recursive_timed_mutex", "std::shared_mutex", "std::shared_timed_mutex", // mutex
        "std::lock_guard", "std::unique_lock", "std::scoped_lock", "std::shared_lock", // locking
        "std::condition_variable", // condition variables
        "std::promise", "std::future", "std::shared_future", // futures
        "std::for_each", "std::reduce", "std::accumulate", "std::execution", // parallel algorithms
        "std::latch", "std::barrier", // synchronization
        
        // pthreads
        "pthread_t", "pthread_mutex_t", "pthread_cond_t", "pthread_rwlock_t",
        "pthread_create", "pthread_join", "pthread_detach", "pthread_exit", "pthread_cancel", "pthread_kill",
        "pthread_mutex_init", "pthread_mutex_destroy", "pthread_mutex_lock", "pthread_mutex_trylock", "pthread_mutex_unlock", "pthread_mutex_timedlock", "pthread_mutex_consistent",
        "pthread_rwlock_init", "pthread_rwlock_destroy", "pthread_rwlock_rdlock", "pthread_rwlock_tryrdlock", "pthread_rwlock_wrlock", "pthread_rwlock_trywrlock", "pthread_rwlock_unlock", "pthread_rwlock_timedrdlock", "pthread_rwlock_timedwrlock",
        "pthread_cond_init", "pthread_cond_destroy", "pthread_cond_signal", "pthread_cond_broadcast", "pthread_cond_wait", "pthread_cond_timedwait", 
        "pthread_condattr_init", "pthread_condattr_destroy", "pthread_condattr_getpshared", "pthread_condattr_setpshared", "pthread_condattr_getclock", "pthread_condattr_setclock",
        
        // openmp
        "omp_lock_t", "omp_nest_lock_t", "omp_sched_t", "omp_sched_t", "omp_proc_bind_t",
        "omp_init_lock", "omp_init_nest_lock", "omp_destroy_lock", "omp_destroy_nest_lock", "omp_set_lock", "omp_set_nest_lock", "omp_unset_lock", "omp_unset_nest_lock",
        "omp_get_thread_num", "omp_get_num_threads", "omp_get_max_threads", "omp_get_num_procs",
        "omp_in_parallel", "omp_set_dynamic", "omp_get_dynamic", "omp_set_nested", "omp_get_nested",
        // "___kmpc_critical", "___kmpc_end_critical", "___kmpc_barrier", "___kmpc_critical_name",
        // "___kmpc_end_critical_name", "___kmpc_atomic_start", "___kmpc_atomic_end",
        // "___kmpc_fork_call", "___kmpc_for_static_init_4", "___kmpc_for_static_init_4u",
        // "GOMP_parallel_start", "GOMP_parallel_end", "GOMP_barrier", "GOMP_critical_start", "GOMP_critical_end",
        // "GOMP_critical_name_start", "GOMP_critical_name_end", "GOMP_atomic_start", "GOMP_atomic_end",
        // "GOMP_loop_static_start", "GOMP_loop_dynamic_start", "GOMP_loop_guided_start", "GOMP_loop_runtime_start",
        // "GOMP_loop_ordered_static_start", "GOMP_loop_ordered_dynamic_start", "GOMP_loop_ordered_guided_start",
        // "GOMP_loop_ordered_runtime_start", "GOMP_loop_end", "GOMP_loop_end_nowait", "GOMP_loop_end_cancel",

        // MPI
        // MPI Caching Functions
        "MPI_Comm_create_keyval", "MPI_Comm_delete_attr", "MPI_Comm_free_keyval", "MPI_Comm_get_attr", "MPI_Comm_set_attr", "MPI_Comm_copy_attr_function", "MPI_Comm_delete_attr_function", "MPI_Type_copy_attr_function", "MPI_Type_delete_attr_function", "MPI_Win_copy_attr_function", "MPI_Win_create_keyval", "MPI_Win_delete_attr", "MPI_Win_delete_attr_function", "MPI_Win_free_keyval", "MPI_Win_get_attr", "MPI_Win_set_attr", "MPI_Type_create_keyval", "MPI_Type_delete_attr", "MPI_Type_free_keyval", "MPI_Type_get_attr", "MPI_Type_set_attr",
        // MPI Collective Functions
        "MPI_Allgather", "MPI_Allgatherv", "MPI_Allreduce", "MPI_Alltoall", "MPI_Alltoallv", "MPI_Alltoallw", "MPI_Barrier", "MPI_Bcast", "MPI_Gather", "MPI_Gatherv", "MPI_Iallgather", "MPI_Iallreduce", "MPI_Ibarrier", "MPI_Ibcast", "MPI_Igather", "MPI_Igatherv", "MPI_Ireduce", "MPI_Iscatter", "MPI_Iscatterv", "MPI_Reduce", "MPI_Scatter", "MPI_Scatterv", "MPI_Exscan", "MPI_Op_create", "MPI_Op_free", "MPI_Reduce_local", "MPI_Reduce_scatter", "MPI_Scan", "MPI_User_function",
        // MPI Communicator Functions
        "MPI_Comm_compare", "MPI_Comm_create", "MPI_Comm_dup", "MPI_Comm_free", "MPI_Comm_rank", "MPI_Comm_size", "MPI_Comm_split", "MPI_Comm_remote_group", "MPI_Comm_remote_size", "MPI_Comm_test_inter", "MPI_Intercomm_create", "MPI_Intercomm_merge",
        // MPI Datatype Functions
        "MPI_Get_address", "MPI_Get_elements", "MPI_Pack", "MPI_Pack_external", "MPI_Pack_external_size", "MPI_Pack_size", "MPI_Type_commit", "MPI_Type_contiguous", "MPI_Type_create_darray", "MPI_Type_create_hindexed", "MPI_Type_create_hindexed_block", "MPI_Type_create_hvector", "MPI_Type_create_indexed_block", "MPI_Type_create_resized", "MPI_Type_create_struct", "MPI_Type_create_subarray", "MPI_Type_dup", "MPI_Type_free", "MPI_Type_get_contents", "MPI_Type_get_envelope", "MPI_Type_get_extent", "MPI_Type_get_true_extent", "MPI_Type_indexed", "MPI_Type_size", "MPI_Type_vector", "MPI_Unpack", "MPI_Unpack_external",
        // MPI Group Functions
        "MPI_Comm_group", "MPI_Group_compare", "MPI_Group_difference", "MPI_Group_excl", "MPI_Group_free", "MPI_Group_incl", "MPI_Group_intersection", "MPI_Group_range_excl", "MPI_Group_range_incl", "MPI_Group_rank", "MPI_Group_size", "MPI_Group_translate_ranks", "MPI_Group_union",
        // MPI Point to Point Functions
        "MPI_Bsend", "MPI_Bsend_init", "MPI_Cancel", "MPI_Get_count", "MPI_Ibsend", "MPI_Iprobe", "MPI_Improbe", "MPI_Imrecv", "MPI_Irecv", "MPI_Irsend", "MPI_Isend", "MPI_Issend", "MPI_Mprobe", "MPI_Mrecv", "MPI_Probe", "MPI_Recv", "MPI_Recv_init", "MPI_Request_free", "MPI_Request_get_status", "MPI_Rsend", "MPI_Rsend_init", "MPI_Send", "MPI_Send_init", "MPI_Sendrecv", "MPI_Sendrecv_replace", "MPI_Ssend", "MPI_Ssend_init", "MPI_Start", "MPI_Startall", "MPI_Test", "MPI_Test_cancelled", "MPI_Testall", "MPI_Testany", "MPI_Testsome", "MPI_Wait", "MPI_Waitall", "MPI_Waitany", "MPI_Waitsome", "MSMPI_Queuelock_acquire", "MSMPI_Queuelock_release", "MSMPI_Waitsome_interruptible",
        // MPI Process Topology Functions
        "MPI_Cart_coords", "MPI_Cart_create", "MPI_Cart_get", "MPI_Cart_map", "MPI_Cart_rank", "MPI_Cart_shift", "MPI_Cart_sub", "MPI_Cartdim_get", "MPI_Dims_create", "MPI_Dist_graph_create", "MPI_Dist_graph_create_adjacent", "MPI_Dist_graph_neighbors", "MPI_Dist_graph_neighbors_count", "MPI_Graph_create", "MPI_Graph_get", "MPI_Graph_map", "MPI_Graph_neighbors", "MPI_Graph_neighbors_count", "MPI_Graphdims_get",
        // MPI Management Functions
        "MPI_Abort", "MPI_Add_error_class", "MPI_Add_error_code", "MPI_Add_error_string", "MPI_Alloc_mem", "MPI_Comm_call_errhandler", "MPI_Comm_create_errhandler", "MPI_Comm_errhandler_fn", "MPI_Comm_get_errhandler", "MPI_Comm_set_errhandler", "MPI_Errhandler_free", "MPI_Error_class", "MPI_Error_string", "MPI_File_call_errhandler", "MPI_File_create_errhandler", "MPI_File_errhandler_fn", "MPI_File_get_errhandler", "MPI_File_set_errhandler", "MPI_Finalize", "MPI_Finalized", "MPI_Free_mem", "MPI_Get_processor_name", "MPI_Get_version", "MPI_Init", "MPI_Initialized", "MPI_Win_call_errhandler", "MPI_Win_create_errhandler", "MPI_Win_errhandler_fn", "MPI_Win_get_errhandler", "MPI_Win_set_errhandler", "MPI_Wtick", "MPI_Wtime",
        // MPI Info Object Functions
        "MPI_Info_create", "MPI_Info_delete", "MPI_Info_dup", "MPI_Info_free", "MPI_Info_get", "MPI_Info_get_nkeys", "MPI_Info_get_nthkey", "MPI_Info_get_valuelen", "MPI_Info_set",
        // MPI Process Management Functions
        "MPI_Close_port", "MPI_Comm_accept", "MPI_Comm_connect", "MPI_Comm_disconnect", "MPI_Comm_get_parent", "MPI_Comm_join", "MPI_Comm_spawn", "MPI_Comm_spawn_multiple", "MPI_Lookup_name", "MPI_Open_port", "MPI_Publish_name", "MPI_Unpublish_name",
        // MPI One-Sided Communications Functions
        "MPI_Accumulate", "MPI_Compare_and_swap", "MPI_Fetch_and_op", "MPI_Get", "MPI_Get_accumulate", "MPI_Raccumulate", "MPI_Rget", "MPI_Rget_accumulate", "MPI_Rput", "MPI_Put", "MPI_Win_allocate", "MPI_Win_allocate_shared", "MPI_Win_attach", "MPI_Win_complete", "MPI_Win_create", "MPI_Win_create_dynamic", "MPI_Win_detach", "MPI_Win_fence", "MPI_Win_flush", "MPI_Win_flush_all", "MPI_Win_flush_local", "MPI_Win_flush_local_all", "MPI_Win_free", "MPI_Win_get_group", "MPI_Win_lock", "MPI_Win_lock_all", "MPI_Win_post", "MPI_Win_shared_query", "MPI_Win_start", "MPI_Win_sync", "MPI_Win_test", "MPI_Win_unlock", "MPI_Win_unlock_all", "MPI_Win_wait",

        // TBB
        "tbb::task_scheduler_init", "tbb::task_group", "tbb::parallel_for", "tbb::parallel_for_each",
        "tbb::parallel_invoke", "tbb::parallel_reduce", "tbb::parallel_scan", "tbb::parallel_sort",
        "tbb::parallel_pipeline", "tbb::task", 
        "tbb::mutex::scoped_lock", "tbb::recursive_mutex::scoped_lock", "tbb::queuing_mutex::scoped_lock", "tbb::queuing_rw_mutex::scoped_lock", "tbb::spin_mutex::scoped_lock", "tbb::spin_rw_mutex::scoped_lock", "tbb::null_mutex::scoped_lock", "tbb::null_rw_mutex::scoped_lock", "tbb::speculative_spin_mutex::scoped_lock", "tbb::speculative_spin_rw_mutex::scoped_lock", "tbb::spin_rw_mutex::scoped_lock", "tbb::rw_mutex::scoped_lock",
        "tbb::mutex", "tbb::recursive_mutex", "tbb::spin_mutex", "tbb::queuing_mutex", "tbb::queuing_rw_mutex", "tbb::spin_rw_mutex", "tbb::null_mutex", "tbb::null_rw_mutex", "tbb::speculative_spin_mutex", "tbb::speculative_spin_rw_mutex", "tbb::spin_rw_mutex", "tbb::rw_mutex",
        
        // Boost
        "boost::thread", "boost::thread_group", "boost::thread_specific_ptr",
        "boost::mutex", "boost::recursive_mutex", "boost::shared_mutex", "boost::unique_lock",
        "boost::shared_lock", "boost::lock_guard", "boost::condition_variable",
        "boost::condition_variable_any", "boost::promise", "boost::future", "boost::shared_future",
        "boost::barrier", "boost::latch", "boost::countdown_latch", "boost::packaged_task",
        "boost::async", "boost::launch",
        "boost::lockfree::queue", "boost::lockfree::stack", "boost::lockfree::spsc_queue",
        
        // RAJA
        "RAJA::forall", "RAJA::forallN", "RAJA::forall_ordered", "RAJA::forallN_ordered",
        "RAJA::forall_execpol", "RAJA::forallN_execpol", "RAJA::forall_execpol_ordered",
        "RAJA::forallN_execpol_ordered", "RAJA::forall_execpol_collapse", "RAJA::forallN_execpol_collapse",
        "RAJA::dynamic::forall", "RAJA::dynamic::forallN", "RAJA::dynamic::forall_execpol", "RAJA::dynamic::forallN_execpol",
        "RAJA::RangeSegment", "RAJA::RangeStrideSegment", "RAJA::ListSegment", "RAJA::TypedListSegment",
        "RAJA::atomic::atomicAdd", "RAJA::atomic::atomicSub", "RAJA::atomic::atomicMin", "RAJA::atomic::atomicMax", "RAJA::atomic::atomicInc", "RAJA::atomic::atomicIncNotCapped", "RAJA::atomic::atomicIncNotCapped", "RAJA::atomic::atomicIncWrap", "RAJA::atomic::atomicIncWrap", "RAJA::atomic::atomicDec",
        
        // Kokkos
        "Kokkos::parallel_for", "Kokkos::parallel_reduce", "Kokkos::parallel_scan",
        "Kokkos::parallel_for_policy", "Kokkos::parallel_reduce_policy", "Kokkos::parallel_scan_policy",
        "Kokkos::atomic_fetch_add", "Kokkos::atomic_fetch_sub", "Kokkos::atomic_fetch_and", "Kokkos::atomic_fetch_or", "Kokkos::atomic_fetch_xor", "Kokkos::atomic_compare_exchange", "Kokkos::atomic_compare_exchange_strong", "Kokkos::atomic_compare_exchange_weak",
        
        // PPL
        "concurrency::critical_section", "concurrency::reader_writer_lock", "concurrency::reader_writer_lock",
        "concurrency::parallel_for", "concurrency::parallel_for_each", "concurrency::parallel_invoke",
        "concurrency::parallel_reduce", "concurrency::parallel_scan", "concurrency::parallel_sort",
        
        // opencl
        "clCreateCommandQueue", "clCreateContext", "clCreateProgramWithSource", "clCreateKernel",
        
        // cuda
        "cudaMalloc", "cudaFree", "cudaMemcpy", "cudaMemcpyAsync", "cudaMemcpy2D", "cudaMemcpy2DAsync",
        
        // openacc
        "acc_init", "acc_shutdown", "acc_get_num_devices", "acc_get_device_type", "acc_set_device_type",
        
        // sycl
        "cl::sycl::queue", "cl::sycl::buffer", "cl::sycl::device", "cl::sycl::context", "cl::sycl::kernel",
        "cl::sycl::program", "cl::sycl::event", "cl::sycl::nd_range", "cl::sycl::range", "cl::sycl::id",
        "cl::sycl::accessor", "cl::sycl::access::mode", "cl::sycl::access::target", "cl::sycl::access::placeholder",
        "cl::sycl::access::target::global_buffer", "cl::sycl::access::target::constant_buffer",
        "cl::sycl::access::target::local", "cl::sycl::access::target::host_buffer",
        "cl::sycl::access::target::image", "cl::sycl::access::target::image_array"
    ),
    // libc++:
    "thread|atomic|mutex|shared_mutex|conditional_variable|future|algo.h|numeric|latch|barrier|"
    // libstdc++
    // "bits/std_thread.h|std/atomic|bits/std_mutex.h|std/shared_mutex|std/condition_variable|std/future|std/parallel/algo.h|"
    // pthreads
    "pthread.h|"
    // openmp
    "omp.h|"
    // mpi
    "mpi.h|"
    // TBB
    "tbb/task_scheduler_init.h|tbb/task_group.h|tbb/parallel_for.h|tbb/parallel_for_each.h|"
    "tbb/parallel_invoke.h|tbb/parallel_reduce.h|tbb/parallel_scan.h|tbb/parallel_sort.h|"
    "tbb/parallel_pipeline.h|tbb/task.h|"
    "tbb/mutex.h|tbb/recursive_mutex.h|tbb/spin_mutex.h|tbb/queuing_mutex.h|tbb/spin_rw_mutex.h|tbb/null_mutex.h|tbb/null_rw_mutex.h|tbb/speculative_spin_mutex.h|tbb/speculative_spin_rw_mutex.h|tbb/spin_rw_mutex.h|tbb/rw_mutex.h|"
    // Boost
    "boost/thread.hpp|boost/thread/group.hpp|boost/thread/tss.hpp|boost/thread/mutex.hpp|"
    "boost/thread/recursive_mutex.hpp|boost/thread/shared_mutex.hpp|boost/thread/lock_guard.hpp|"
    "boost/thread/shared_lock_guard.hpp|boost/thread/condition_variable.hpp|"
    "boost/thread/condition_variable_any.hpp|boost/thread/pthread/barrier.hpp|"
    "boost/thread/latch.hpp|boost/thread/countdown_latch.hpp|boost/thread/packaged_task.hpp|"
    "boost/thread/future.hpp|boost/thread/shared_future.hpp|boost/thread/async.hpp|"
    "boost/thread/launch.hpp|"
    "boost/lockfree/queue.hpp|boost/lockfree/stack.hpp|boost/lockfree/spsc_queue.hpp|"
    // RAJA
    "RAJA.hpp|RAJA/pattern/forall.hpp|RAJA/pattern/forallN.hpp|RAJA/pattern/forall_ordered.hpp|"
    // Kokkos
    "Kokkos_Core.hpp|Kokkos_Parallel.hpp|Kokkos_Atomic.hpp|"
    // PPL
    "ppl.h|"
    // opencl
    "CL/cl.h|"
    // cuda
    "cuda_runtime.h|"
    // openacc
    "openacc.h|"
    // sycl
    "CL/sycl.hpp"
) {}


