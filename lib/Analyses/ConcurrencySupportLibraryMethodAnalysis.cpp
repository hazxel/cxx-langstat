#include "cxx-langstat/Analyses/ConcurrencySupportLibraryMethodAnalysis.h"
#include "cxx-langstat/Utils.h"

using ordered_json = nlohmann::ordered_json;

ConcurrencySupportLibraryMethodAnalysis::ConcurrencySupportLibraryMethodAnalysis() : ClassMethodCallAnalysis(
        clang::ast_matchers::hasAnyName(
        // standard library concurrency supports
        "std::thread", "std::jthread", // threads
        "std::atomic", // atomic operations
        "std::mutex", "std::lock_guard", "std::unique_lock", // locking
        "std::condition_variable", // condition variables
        "std::promise", "std::future", "std::shared_future", // futures
        "std::for_each", "std::reduce", "std::accumulate", "std::execution", // parallel algorithms
        "std::latch", "std::barrier", // synchronization
        "std::vector", "std::array", "std::list", "std::map", "std::unordered_map", "std::set", "std::unordered_set", "std::queue", "std::deque", "std::priority_queue", "std::stack", // containers
        // pthreads
        "pthread_t", "pthread_mutex_t", "pthread_cond_t", "pthread_rwlock_t",
        "pthread_create", "pthread_join", "pthread_detach", "pthread_exit", "pthread_cancel",
        "pthread_mutex_init", "pthread_mutex_destroy", "pthread_mutex_lock", "pthread_mutex_trylock",
        "pthread_mutex_unlock", "pthread_mutex_timedlock", "pthread_mutex_consistent",
        // openmp
        "omp_lock_t", "omp_nest_lock_t", "omp_sched_t", "omp_sched_t", "omp_proc_bind_t",
        "omp_init_lock", "omp_init_nest_lock", "omp_destroy_lock", "omp_destroy_nest_lock",
        "omp_set_lock", "omp_set_nest_lock", "omp_unset_lock", "omp_unset_nest_lock",
        "omp_get_thread_num", "omp_get_num_threads", "omp_get_max_threads", "omp_get_num_procs",
        // "___kmpc_critical", "___kmpc_end_critical", "___kmpc_barrier", "___kmpc_critical_name",
        // "___kmpc_end_critical_name", "___kmpc_atomic_start", "___kmpc_atomic_end",
        // "___kmpc_fork_call", "___kmpc_for_static_init_4", "___kmpc_for_static_init_4u",
        // "GOMP_parallel_start", "GOMP_parallel_end", "GOMP_barrier", "GOMP_critical_start", "GOMP_critical_end",
        // "GOMP_critical_name_start", "GOMP_critical_name_end", "GOMP_atomic_start", "GOMP_atomic_end",
        // "GOMP_loop_static_start", "GOMP_loop_dynamic_start", "GOMP_loop_guided_start", "GOMP_loop_runtime_start",
        // "GOMP_loop_ordered_static_start", "GOMP_loop_ordered_dynamic_start", "GOMP_loop_ordered_guided_start",
        // "GOMP_loop_ordered_runtime_start", "GOMP_loop_end", "GOMP_loop_end_nowait", "GOMP_loop_end_cancel",
        // mpi
        "MPI_Init", "MPI_Finalize", "MPI_Comm_size", "MPI_Comm_rank", "MPI_Send", "MPI_Recv",
        // TBB
        "tbb::task_scheduler_init", "tbb::task_group", "tbb::parallel_for", "tbb::parallel_for_each",
        "tbb::parallel_invoke", "tbb::parallel_reduce", "tbb::parallel_scan", "tbb::parallel_sort",
        "tbb::parallel_pipeline", "tbb::task", "tbb::scoped_lock", "tbb::mutex", "tbb::spin_mutex",
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
        "RAJA::RangeSegment", "RAJA::RangeStrideSegment", "RAJA::ListSegment", "RAJA::TypedListSegment",
        // Kokkos
        "Kokkos::parallel_for", "Kokkos::parallel_reduce", "Kokkos::parallel_scan",
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
    "thread|atomic|mutex|conditional_variable|future|algo.h|numeric|"
    // libstdc++
    // "bits/std_thread.h|std/atomic|bits/std_mutex.h|std/condition_variable|std/future|std/parallel/algo.h|"
    // pthreads
    "pthread.h|"
    // openmp
    "omp.h|"
    // mpi
    "mpi.h|"
    // TBB
    "tbb/task_scheduler_init.h|tbb/task_group.h|tbb/parallel_for.h|tbb/parallel_for_each.h|"
    "tbb/parallel_invoke.h|tbb/parallel_reduce.h|tbb/parallel_scan.h|tbb/parallel_sort.h|"
    "tbb/parallel_pipeline.h|tbb/task.h|tbb/mutex.h|tbb/spin_mutex.h|"
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
    "RAJA.hpp|RAJA/pattern/forall.hpp|"
    // Kokkos
    "Kokkos_Core.hpp|Kokkos_Parallel.hpp|"
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


