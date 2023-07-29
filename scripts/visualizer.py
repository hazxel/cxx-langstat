import os
import json
import random
import matplotlib.pyplot as plt

prefix = '/Users/boyan/Documents/ETH-Courses/Thesis/statistics-out'    

constructors = {}
methods = {}
functions = {}

lib_usage = {}

primitive_usage = {}
thread_usage = {}
parallel_usage = {}
lock_mutex_usage = {}
condition_variable_usage = {}
atomic_usage = {}
atomic_memroy_order_function_usage = {}

std_lib_usage = {}
boost_lib_usage = {}    
openmp_lib_usage = {}
tbb_lib_usage = {}
pthread_lib_usage = {}
mpi_lib_usage = {}
mpi_category_usage = {}
kokkos_lib_usage = {}
raja_lib_usage = {}
cuda_lib_usage = {}

project_lib_usage = {}

container_usage = {}
container_lock_mutex_usage = {}


analysis = "cslma"
for filename in os.listdir(prefix + '/' + analysis):
    if not filename.endswith(".json"):
        continue

    projectname = filename.split('.')[0]
    project_lib_usage[projectname] = project_lib_usage.get(projectname, {})

    f = open(prefix + '/' + analysis + '/' + filename, 'r')
    data = json.load(f)

    # instance constructions
    for constructor_name, calls in data["Summary"]["constructor calls"].items():
        constructors[str(constructor_name)] = constructors.get(constructor_name, 0) + calls
        if "std::" in constructor_name:
            if "lock" in constructor_name:
                lock_mutex_usage["STL"] = lock_mutex_usage.get("STL", 0) + calls
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + calls
                lib_usage["STL"] = lib_usage.get("STL", 0) + calls
                project_lib_usage[projectname]["STL"] = project_lib_usage[projectname].get("STL", 0) + calls
                std_lib_usage["mutex lock"] = std_lib_usage.get("mutex lock", 0) + calls

    # method call via instances
    for instance_name, calls in data["Summary"]["method calls"].items():
        sum = 0
        for method_name, call in calls.items():
            if instance_name not in methods:
                methods[instance_name] = {}
            methods[instance_name][method_name] = methods.get(instance_name, {}).get(method_name, 0) + call
            sum += call
        
        if "std::" in instance_name:
            lib_usage["STL"] = lib_usage.get("STL", 0) + sum
            project_lib_usage[projectname]["STL"] = project_lib_usage[projectname].get("STL", 0) + sum
            if "thread" in instance_name:
                thread_usage["STL"] = thread_usage.get("STL", 0) + sum
                primitive_usage["thread"] = primitive_usage.get("thread", 0) + sum
                std_lib_usage["thread"] = std_lib_usage.get("thread", 0) + sum
            elif "mutex" in instance_name or "lock" in instance_name:
                lock_mutex_usage["STL"] = lock_mutex_usage.get("STL", 0) + sum
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + sum
                std_lib_usage["mutex lock"] = std_lib_usage.get("mutex lock", 0) + sum
            elif "atomic" in instance_name:
                atomic_usage["STL"] = atomic_usage.get("STL", 0) + sum
                primitive_usage["atomic"] = primitive_usage.get("atomic", 0) + sum
                std_lib_usage["atomic"] = std_lib_usage.get("atomic", 0) + sum
            elif "condition_variable" in instance_name:
                condition_variable_usage["STL"] = condition_variable_usage.get("STL", 0) + sum
                primitive_usage["condition variable"] = primitive_usage.get("condition variable", 0) + sum
                std_lib_usage["condition variable"] = std_lib_usage.get("condition variable", 0) + sum
        elif "boost::" in instance_name:
            lib_usage["Boost"] = lib_usage.get("Boost", 0) + sum
            project_lib_usage[projectname]["Boost"] = project_lib_usage[projectname].get("Boost", 0) + sum
            if "thread" in instance_name:
                thread_usage["Boost"] = thread_usage.get("Boost", 0) + sum
                primitive_usage["thread"] = primitive_usage.get("thread", 0) + sum
                boost_lib_usage["thread"] = boost_lib_usage.get("thread", 0) + sum
            elif "mutex" in instance_name or "lock" in instance_name:
                lock_mutex_usage["Boost"] = lock_mutex_usage.get("Boost", 0) + sum
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + sum
                boost_lib_usage["mutex lock"] = boost_lib_usage.get("mutex lock", 0) + sum
            elif "atomic" in instance_name:
                atomic_usage["Boost"] = atomic_usage.get("Boost", 0) + sum
                primitive_usage["atomic"] = primitive_usage.get("atomic", 0) + sum
                boost_lib_usage["atomic"] = boost_lib_usage.get("atomic", 0) + sum
        elif "omp_" in instance_name:
            lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + sum
            project_lib_usage[projectname]["OpenMP"] = project_lib_usage[projectname].get("OpenMP", 0) + sum
            if "lock" in instance_name:
                lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + sum
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + sum
                openmp_lib_usage["mutex lock"] = openmp_lib_usage.get("mutex lock", 0) + sum
        elif "tbb::" in instance_name:
            lib_usage["TBB"] = lib_usage.get("TBB", 0) + sum
            project_lib_usage[projectname]["TBB"] = project_lib_usage[projectname].get("TBB", 0) + sum
            if "mutex" in instance_name or "lock" in instance_name:
                lock_mutex_usage["TBB"] = lock_mutex_usage.get("TBB", 0) + sum
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + sum
                tbb_lib_usage["mutex lock"] = tbb_lib_usage.get("mutex lock", 0) + sum
            elif "task" in instance_name:
                pass
        else:
            lib_usage["other"] = lib_usage.get("other", 0) + sum
            project_lib_usage[projectname]["other"] = project_lib_usage[projectname].get("other", 0) + sum
            print(projectname, instance_name)

    # function calls
    for function_name, calls in data["Summary"]["function calls"].items():
        functions[function_name] = functions.get(function_name, 0) + calls
        if "std::" in function_name:
            lib_usage["STL"] = lib_usage.get("STL", 0) + calls
            project_lib_usage[projectname]["STL"] = project_lib_usage[projectname].get("STL", 0) + calls
            if "for_each" in function_name:
                parallel_usage["STL"] = parallel_usage.get("STL", 0) + calls
                primitive_usage["high-level thread"] = primitive_usage.get("high-level thread", 0) + calls
                std_lib_usage["high-level thread"] = std_lib_usage.get("high-level thread", 0) + calls
        elif "pthread_" in function_name:
            lib_usage["pthread"] = lib_usage.get("pthread", 0) + calls
            project_lib_usage[projectname]["pthread"] = project_lib_usage[projectname].get("pthread", 0) + calls
            if "mutex" in function_name:
                lock_mutex_usage["pthread"] = lock_mutex_usage.get("pthread", 0) + calls
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + calls
                pthread_lib_usage["mutex lock"] = pthread_lib_usage.get("mutex lock", 0) + calls
            elif "cond" in function_name:
                condition_variable_usage["pthread"] = condition_variable_usage.get("pthread", 0) + calls
                primitive_usage["condition variable"] = primitive_usage.get("condition variable", 0) + calls
                pthread_lib_usage["condition variable"] = pthread_lib_usage.get("condition variable", 0) + calls
            else:
                thread_usage["pthread"] = thread_usage.get("pthread", 0) + calls
                primitive_usage["thread"] = primitive_usage.get("thread", 0) + calls
                pthread_lib_usage["thread"] = pthread_lib_usage.get("thread", 0) + calls
        elif "MPI_" in function_name:
            mpi_lib_usage[function_name] = mpi_lib_usage.get(function_name, 0) + calls
            if function_name in ["MPI_Comm_create_keyval", "MPI_Comm_delete_attr", "MPI_Comm_free_keyval", "MPI_Comm_get_attr", "MPI_Comm_set_attr", "MPI_Comm_copy_attr_function", "MPI_Comm_delete_attr_function", "MPI_Type_copy_attr_function", "MPI_Type_delete_attr_function", "MPI_Win_copy_attr_function", "MPI_Win_create_keyval", "MPI_Win_delete_attr", "MPI_Win_delete_attr_function", "MPI_Win_free_keyval", "MPI_Win_get_attr", "MPI_Win_set_attr", "MPI_Type_create_keyval", "MPI_Type_delete_attr", "MPI_Type_free_keyval", "MPI_Type_get_attr", "MPI_Type_set_attr"]:
                mpi_category_usage["Caching"] = mpi_category_usage.get("Caching", 0) + calls
            elif function_name in ["MPI_Allgather", "MPI_Allgatherv", "MPI_Allreduce", "MPI_Alltoall", "MPI_Alltoallv", "MPI_Alltoallw", "MPI_Barrier", "MPI_Bcast", "MPI_Gather", "MPI_Gatherv", "MPI_Iallgather", "MPI_Iallreduce", "MPI_Ibarrier", "MPI_Ibcast", "MPI_Igather", "MPI_Igatherv", "MPI_Ireduce", "MPI_Iscatter", "MPI_Iscatterv", "MPI_Reduce", "MPI_Scatter", "MPI_Scatterv", "MPI_Exscan", "MPI_Op_create", "MPI_Op_free", "MPI_Reduce_local", "MPI_Reduce_scatter", "MPI_Scan", "MPI_User_function"]:
                mpi_category_usage["Collective"] = mpi_category_usage.get("Collective", 0) + calls
            elif function_name in ["MPI_Comm_compare", "MPI_Comm_create", "MPI_Comm_dup", "MPI_Comm_free", "MPI_Comm_rank", "MPI_Comm_size", "MPI_Comm_split", "MPI_Comm_remote_group", "MPI_Comm_remote_size", "MPI_Comm_test_inter", "MPI_Intercomm_create", "MPI_Intercomm_merge"]:
                mpi_category_usage["Communicator"] = mpi_category_usage.get("Communicator", 0) + calls
            elif function_name in ["MPI_Get_address", "MPI_Get_elements", "MPI_Pack", "MPI_Pack_external", "MPI_Pack_external_size", "MPI_Pack_size", "MPI_Type_commit", "MPI_Type_contiguous", "MPI_Type_create_darray", "MPI_Type_create_hindexed", "MPI_Type_create_hindexed_block", "MPI_Type_create_hvector", "MPI_Type_create_indexed_block", "MPI_Type_create_resized", "MPI_Type_create_struct", "MPI_Type_create_subarray", "MPI_Type_dup", "MPI_Type_free", "MPI_Type_get_contents", "MPI_Type_get_envelope", "MPI_Type_get_extent", "MPI_Type_get_true_extent", "MPI_Type_indexed", "MPI_Type_size", "MPI_Type_vector", "MPI_Unpack", "MPI_Unpack_external"]:
                mpi_category_usage["Datatype"] = mpi_category_usage.get("Datatype", 0) + calls
            elif function_name in ["MPI_Comm_group", "MPI_Group_compare", "MPI_Group_difference", "MPI_Group_excl", "MPI_Group_free", "MPI_Group_incl", "MPI_Group_intersection", "MPI_Group_range_excl", "MPI_Group_range_incl", "MPI_Group_rank", "MPI_Group_size", "MPI_Group_translate_ranks", "MPI_Group_union"]:
                mpi_category_usage["Group"] = mpi_category_usage.get("Group", 0) + calls
            elif function_name in ["MPI_Bsend", "MPI_Bsend_init", "MPI_Cancel", "MPI_Get_count", "MPI_Ibsend", "MPI_Iprobe", "MPI_Improbe", "MPI_Imrecv", "MPI_Irecv", "MPI_Irsend", "MPI_Isend", "MPI_Issend", "MPI_Mprobe", "MPI_Mrecv", "MPI_Probe", "MPI_Recv", "MPI_Recv_init", "MPI_Request_free", "MPI_Request_get_status", "MPI_Rsend", "MPI_Rsend_init", "MPI_Send", "MPI_Send_init", "MPI_Sendrecv", "MPI_Sendrecv_replace", "MPI_Ssend", "MPI_Ssend_init", "MPI_Start", "MPI_Startall", "MPI_Test", "MPI_Test_cancelled", "MPI_Testall", "MPI_Testany", "MPI_Testsome", "MPI_Wait", "MPI_Waitall", "MPI_Waitany", "MPI_Waitsome", "MSMPI_Queuelock_acquire", "MSMPI_Queuelock_release", "MSMPI_Waitsome_interruptible"]:
                mpi_category_usage["Point-to-Point"] = mpi_category_usage.get("Point-to-Point", 0) + calls
            elif function_name in ["MPI_Cart_coords", "MPI_Cart_create", "MPI_Cart_get", "MPI_Cart_map", "MPI_Cart_rank", "MPI_Cart_shift", "MPI_Cart_sub", "MPI_Cartdim_get", "MPI_Dims_create", "MPI_Dist_graph_create", "MPI_Dist_graph_create_adjacent", "MPI_Dist_graph_neighbors", "MPI_Dist_graph_neighbors_count", "MPI_Graph_create", "MPI_Graph_get", "MPI_Graph_map", "MPI_Graph_neighbors", "MPI_Graph_neighbors_count", "MPI_Graphdims_get"]:
                mpi_category_usage["Proc-Topo"] = mpi_category_usage.get("Proc-Topo", 0) + calls
            elif function_name in ["MPI_Abort", "MPI_Add_error_class", "MPI_Add_error_code", "MPI_Add_error_string", "MPI_Alloc_mem", "MPI_Comm_call_errhandler", "MPI_Comm_create_errhandler", "MPI_Comm_errhandler_fn", "MPI_Comm_get_errhandler", "MPI_Comm_set_errhandler", "MPI_Errhandler_free", "MPI_Error_class", "MPI_Error_string", "MPI_File_call_errhandler", "MPI_File_create_errhandler", "MPI_File_errhandler_fn", "MPI_File_get_errhandler", "MPI_File_set_errhandler", "MPI_Finalize", "MPI_Finalized", "MPI_Free_mem", "MPI_Get_processor_name", "MPI_Get_version", "MPI_Init", "MPI_Initialized", "MPI_Win_call_errhandler", "MPI_Win_create_errhandler", "MPI_Win_errhandler_fn", "MPI_Win_get_errhandler", "MPI_Win_set_errhandler", "MPI_Wtick", "MPI_Wtime"]:
                mpi_category_usage["Management"] = mpi_category_usage.get("Management", 0) + calls
            elif function_name in ["MPI_Info_create", "MPI_Info_delete", "MPI_Info_dup", "MPI_Info_free", "MPI_Info_get", "MPI_Info_get_nkeys", "MPI_Info_get_nthkey", "MPI_Info_get_valuelen", "MPI_Info_set"]:
                mpi_category_usage["Info-Obj"] = mpi_category_usage.get("Info-Obj", 0) + calls
            elif function_name in ["MPI_Close_port", "MPI_Comm_accept", "MPI_Comm_connect", "MPI_Comm_disconnect", "MPI_Comm_get_parent", "MPI_Comm_join", "MPI_Comm_spawn", "MPI_Comm_spawn_multiple", "MPI_Lookup_name", "MPI_Open_port", "MPI_Publish_name", "MPI_Unpublish_name"]:
                mpi_category_usage["Proc-Mgmt"] = mpi_category_usage.get("Proc-Mgmt", 0) + calls
            elif function_name in ["MPI_Accumulate", "MPI_Compare_and_swap", "MPI_Fetch_and_op", "MPI_Get", "MPI_Get_accumulate", "MPI_Raccumulate", "MPI_Rget", "MPI_Rget_accumulate", "MPI_Rput", "MPI_Put", "MPI_Win_allocate", "MPI_Win_allocate_shared", "MPI_Win_attach", "MPI_Win_complete", "MPI_Win_create", "MPI_Win_create_dynamic", "MPI_Win_detach", "MPI_Win_fence", "MPI_Win_flush", "MPI_Win_flush_all", "MPI_Win_flush_local", "MPI_Win_flush_local_all", "MPI_Win_free", "MPI_Win_get_group", "MPI_Win_lock", "MPI_Win_lock_all", "MPI_Win_post", "MPI_Win_shared_query", "MPI_Win_start", "MPI_Win_sync", "MPI_Win_test", "MPI_Win_unlock", "MPI_Win_unlock_all", "MPI_Win_wait"]:
                mpi_category_usage["Oneside-Comm"] = mpi_category_usage.get("Oneside-Comm", 0) + calls
            if "MPI_I" in function_name \
                or "MPI_Finalize" in function_name \
                or "MPI_Comm_" in function_name \
                or "MPI_Get_" in function_name:
                continue
            lib_usage["MPI"] = lib_usage.get("MPI", 0) + calls
            project_lib_usage[projectname]["MPI"] = project_lib_usage[projectname].get("MPI", 0) + calls
        elif "omp_" in function_name:
            lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + calls
            project_lib_usage[projectname]["OpenMP"] = project_lib_usage[projectname].get("OpenMP", 0) + calls
            if "lock" in instance_name:
                lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + sum
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + sum
                openmp_lib_usage["mutex lock"] = openmp_lib_usage.get("mutex lock", 0) + sum
        elif "RAJA::" in function_name:
            lib_usage["RAJA"] = lib_usage.get("RAJA", 0) + calls
            project_lib_usage[projectname]["RAJA"] = project_lib_usage[projectname].get("RAJA", 0) + calls
            if "forall" in function_name:
                parallel_usage["RAJA"] = parallel_usage.get("RAJA", 0) + calls
                primitive_usage["high-level thread"] = primitive_usage.get("high-level thread", 0) + calls
                raja_lib_usage["high-level thread"] = raja_lib_usage.get("high-level thread", 0) + calls
            elif "atomic" in function_name:
                atomic_usage["RAJA"] = atomic_usage.get("RAJA", 0) + calls
                primitive_usage["atomic"] = primitive_usage.get("atomic", 0) + calls
                raja_lib_usage["atomic"] = raja_lib_usage.get("atomic", 0) + calls
            elif "mutex" in function_name:
                lock_mutex_usage["RAJA"] = lock_mutex_usage.get("RAJA", 0) + calls
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + calls
                raja_lib_usage["mutex lock"] = raja_lib_usage.get("mutex lock", 0) + calls
            else:
                raja_lib_usage["other"] = raja_lib_usage.get("other", 0) + calls
        elif "Kokkos::" in function_name:
            lib_usage["Kokkos"] = lib_usage.get("Kokkos", 0) + calls
            project_lib_usage[projectname]["Kokkos"] = project_lib_usage[projectname].get("Kokkos", 0) + calls
            if "parallel_" in function_name:
                parallel_usage["Kokkos"] = parallel_usage.get("Kokkos", 0) + calls
                primitive_usage["high-level thread"] = primitive_usage.get("high-level thread", 0) + calls
                kokkos_lib_usage["high-level thread"] = kokkos_lib_usage.get("high-level thread", 0) + calls
            elif "atomic" in function_name:
                atomic_usage["Kokkos"] = atomic_usage.get("Kokkos", 0) + calls
                primitive_usage["atomic"] = primitive_usage.get("atomic", 0) + calls
                kokkos_lib_usage["atomic"] = kokkos_lib_usage.get("atomic", 0) + calls
            elif "lock" in function_name:
                lock_mutex_usage["Kokkos"] = lock_mutex_usage.get("Kokkos", 0) + calls
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + calls
                kokkos_lib_usage["mutex lock"] = kokkos_lib_usage.get("mutex lock", 0) + calls
            else:
                kokkos_lib_usage[function_name] = kokkos_lib_usage.get(function_name, 0) + calls
        elif "tbb::" in function_name:
            lib_usage["TBB"] = lib_usage.get("TBB", 0) + calls
            project_lib_usage[projectname]["TBB"] = project_lib_usage[projectname].get("TBB", 0) + calls
            if "parallel_" in function_name:
                parallel_usage["TBB"] = parallel_usage.get("TBB", 0) + calls
                primitive_usage["high-level thread"] = primitive_usage.get("high-level thread", 0) + calls
                tbb_lib_usage["high-level thread"] = tbb_lib_usage.get("high-level thread", 0) + calls
            else:
                tbb_lib_usage[function_name] = tbb_lib_usage.get(function_name, 0) + calls
        else:
            lib_usage["other"] = lib_usage.get("other", 0) + calls
            project_lib_usage[projectname]["other"] = project_lib_usage[projectname].get("other", 0) + calls
            print(projectname, function_name)
    f.close()

analysis = "ompeda"
for filename in os.listdir(prefix + '/' + analysis):
    if not filename.endswith(".json"):
        continue

    f = open(prefix + '/' + analysis + '/' + filename, 'r')
    data = json.load(f)

    if data["Summary"]["omp_directive"] == {}:
        continue

    projectname = filename.split('.')[0]
    project_lib_usage[projectname] = project_lib_usage.get(projectname, {})

    for ompdir_name, calls in data["Summary"]["omp_directive"].items():
        if "Parallel" in ompdir_name or "For" in ompdir_name or "OMPMasterDirective" in ompdir_name or "OMPSingleDirective" in ompdir_name:
            parallel_usage["OpenMP"] = parallel_usage.get("OpenMP", 0) + calls
            primitive_usage["high-level thread"] = primitive_usage.get("high-level thread", 0) + calls
            openmp_lib_usage["parallel"] = openmp_lib_usage.get("parallel", 0) + calls
        elif ompdir_name in ["OMPCriticalDirective", "OMPMasterDirective", "OMPSingleDirective", "OMPBarrierDirective", "OMPAtomicDirective", "OMPTaskwaitDirective", "OMPOrderedDirective", "OMPFlushDirective"]:
            openmp_lib_usage["synchronization"] = openmp_lib_usage.get("synchronization", 0) + calls
            if ompdir_name == "OMPCriticalDirective":
                lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + calls
                primitive_usage["mutex lock"] = primitive_usage.get("mutex lock", 0) + calls
            elif ompdir_name == "OMPAtomicDirective":
                atomic_usage["OpenMP"] = atomic_usage.get("OpenMP", 0) + calls
                primitive_usage["atomic"] = primitive_usage.get("atomic", 0) + calls
        elif "Task" in ompdir_name:
            openmp_lib_usage["task"] = openmp_lib_usage.get("task", 0) + calls
        elif "Simd" in ompdir_name:
            openmp_lib_usage["simd"] = openmp_lib_usage.get("simd", 0) + calls
        else:
            openmp_lib_usage[ompdir_name] = openmp_lib_usage.get(ompdir_name, 0) + calls
            continue
        lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + calls
        project_lib_usage[projectname]["OpenMP"] = project_lib_usage[projectname].get("OpenMP", 0) + calls
    f.close()

analysis = "amoa"
for filename in os.listdir(prefix + '/' + analysis):
    if not filename.endswith(".json"):
        continue

    projectname = filename.split('.')[0]
    f = open(prefix + '/' + analysis + '/' + filename, 'r')
    data = json.load(f)

    for function_name, memory_order_count_dict in data["Summary"]["memory_order"].items():
        atomic_memroy_order_function_usage[function_name] = atomic_memroy_order_function_usage.get(function_name, {})
        for memory_order, count in memory_order_count_dict.items():
            memory_order = memory_order.replace("std::memory_order_", "")
            memory_order = memory_order.replace("std::memory_order::memory_order_", "")
            atomic_memroy_order_function_usage[function_name][memory_order] = atomic_memroy_order_function_usage[function_name].get(memory_order, 0) + count
    f.close()

analysis = "dca"
for filename in os.listdir(prefix + '/' + analysis):
    if not filename.endswith(".json"):
        continue
    projectname= filename.split('.')[0]
    f = open(prefix + '/' + analysis + '/' + filename, 'r')
    data = json.load(f)
    for container_name, method_calls in data["Summary"]["container_usage"].items():
        for method_name, calls in method_calls.items():
            if container_name in["std::vector", "std::list", "std::array", "std::map", "std::unordered_map", "std::set"]:
                container_usage[container_name] = container_usage.get(container_name, 0) + calls
            else:
                container_usage["other"] = container_usage.get("other", 0) + calls
    f.close()
container_usage = dict(sorted(container_usage.items(), key=lambda x: x[1], reverse=False))

analysis = "smsa"
set_of_containers = set()
for filename in os.listdir(prefix + '/' + analysis):
    if not filename.endswith(".json"):
        continue
    projectname= filename.split('.')[0]
    f = open(prefix + '/' + analysis + '/' + filename, 'r')
    data = json.load(f)
    for mutex_lock_type, features in data["Summary"]["scope"].items():
        if "std::" in mutex_lock_type:
            if "lock_guard" in mutex_lock_type:
                mutex_lock_type = "std::lock_guard"
            if "unique_lock" in mutex_lock_type:
                mutex_lock_type = "std::unique_lock"
            if "shared_lock" in mutex_lock_type:
                mutex_lock_type = "std::shared_lock"
            if "scoped_lock" in mutex_lock_type:
                mutex_lock_type = "std::scoped_lock"
        container_lock_mutex_usage[mutex_lock_type] = container_lock_mutex_usage.get(mutex_lock_type, {})
        # not deal with function calls
        if "member_method_features" not in features:
            continue
        for container_type_name, method_calls in features["member_method_features"].items():
            if "std::" in container_type_name:
                if "vector" in container_type_name:
                    container_type_name = "std::vector"
                elif "list" in container_type_name:
                    container_type_name = "std::list"
                elif "unordered_map" in container_type_name:
                    container_type_name = "std::unordered_map"
                elif "map" in container_type_name:
                    container_type_name = "std::map"
                elif "set" in container_type_name:
                    container_type_name = "std::set"
                else:
                    container_type_name = "other"

            set_of_containers.add(container_type_name)
            calls = 0
            for method_name, count in method_calls.items():
                calls += count
            container_lock_mutex_usage[mutex_lock_type][container_type_name] = container_lock_mutex_usage[mutex_lock_type].get(container_type_name, 0) + calls
    f.close()
set_of_containers = list(sorted(set_of_containers, reverse=True))







########
# plot #
########


colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']
def plot_feature(
        feature_usage_dict, 
        fig_name,
        x_label_name = "total occurrence",
        y_label_name = ""):
    num_rows = len(feature_usage_dict)
    height = max(0.4, num_rows*0.6 + 2)
    plt.figure(figsize = (10, height))
    plt.ylim(bottom = -0.5, top = len(feature_usage_dict) - 0.5)
    plt.barh(list(feature_usage_dict.keys()), list(feature_usage_dict.values()), height=0.35, align='center', color='maroon')
    plt.xticks(fontsize = 14)
    plt.yticks(fontsize = 16)
    plt.xlabel(x_label_name, fontsize=18)
    plt.ylabel(y_label_name, fontsize=18)
    
    plt.tight_layout()
    plt.savefig(prefix + '/' + fig_name)
    plt.close()

plot_feature(lib_usage, "lib.png")
plot_feature(thread_usage, "thread.png")
plot_feature(lock_mutex_usage, "lock_mutex.png")
plot_feature(parallel_usage, "parallel.png")
plot_feature(atomic_usage, "atomic.png")
plot_feature(condition_variable_usage, "condition_var.png")

plot_feature(primitive_usage, "primitive.png", y_label_name = "primitives")

plot_feature(std_lib_usage, "std_lib.png", y_label_name = "primitives")
plot_feature(openmp_lib_usage, "omp_lib.png", y_label_name = "directives")
plot_feature(pthread_lib_usage, "pthread_lib.png", y_label_name = "primitives")
plot_feature(mpi_lib_usage, "mpi_lib.png", y_label_name = "primitives")
plot_feature(mpi_category_usage, "mpi_category.png", y_label_name = "function categories")
# plot_feature(kokkos_lib_usage, "kokkos_lib.png", y_label_name = "primitives")
# plot_feature(raja_lib_usage, "raja_lib.png", y_label_name = "primitives")
# plot_feature(tbb_lib_usage, "tbb_lib.png", y_label_name = "primitives")
raja_kokkos_tbb = {"TBB": tbb_lib_usage.get("high-level thread", 0), "RAJA": raja_lib_usage.get("high-level thread", 0), "Kokkos": kokkos_lib_usage.get("high-level thread", 0)}
plot_feature(raja_kokkos_tbb, "raja_kokkos_tbb.png", x_label_name="high-level threading construct occurrence")

plot_feature(container_usage, "container.png", y_label_name = "container type")

# statistics for atomic memory order
for function_name, memory_order_count_dict in atomic_memroy_order_function_usage.items():
    plot_feature(memory_order_count_dict, "atomic_memory_order_" + function_name + ".png", x_label_name = "total occurrence", y_label_name = "memory order")

# statistics for all the projects
plt.figure(figsize = (15, 15))
set_of_libs = set()
keys = list(project_lib_usage.keys())
keys.sort(reverse=True)
project_lib_usage = {k: project_lib_usage[k] for k in keys}
for lib_usage in project_lib_usage.values():
    for lib in lib_usage.keys():
        set_of_libs.add(lib)
base=[0] * len(project_lib_usage)
for lib in set_of_libs:
    cur_bar = [lib_usage.get(lib, 0) for lib_usage in project_lib_usage.values()]
    random.shuffle(colors)
    plt.barh(list(project_lib_usage.keys()), cur_bar, height=0.5, align='center', left=base)
    base = [base[i] + cur_bar[i] for i in range(len(base))] 
# plt.title("concurrency support libraries usage of HPC projects")
plt.xticks(fontsize = 20)
plt.yticks(fontsize = 20)
plt.xlabel("total occurrence", fontsize=20)
plt.legend(set_of_libs, fontsize=20)
plt.tight_layout()
plt.savefig(prefix + '/project_lib.png')
plt.close()

# print a latex table text for the project lib usage
keys = list(project_lib_usage.keys())
keys.sort(reverse=False)
project_lib_usage = {k: project_lib_usage[k] for k in keys}
num_of_projects = len(project_lib_usage)
coverage = {}
print("\n\\begin{table}")
print("\\centering")
print("\\begin{tabular}{|l|" + "c|" * len(set_of_libs) + "}")
print("  \\cline{2-" + str(len(set_of_libs)+1) + "}")
print("  \multicolumn{1}{c|}{}")
line = "  "
for lib in set_of_libs:
    line += " & " + lib
print(line + "\\\\")
print("  \\cline{1-" + str(len(set_of_libs)+1) + "}")
for project_name, lib_usage in project_lib_usage.items():
    line = "  " + project_name
    for lib in set_of_libs:
        line += " & "
        if lib_usage.get(lib, 0) != 0:
            line += "\\checkmark"
            coverage[lib] = coverage.get(lib, 0) + 1
    print(line + " \\\\")
print("  \\hline")
line = "  Adoption"
for lib in set_of_libs:
    line += " & " + str(round(coverage[lib] / num_of_projects * 100, 2)) + "\\%"
print(line + " \\\\")
print("  \\hline")
print("\\end{tabular}")
print("\\caption{Adoption of different parallel programming models in HPC applications.}")
print("\\label{table:project_lib}")
print("\\end{table}")

# statistics for mutex lock prevalence in container usage
### in what percentage of each STL container usage, STL/boost/tbb/pthread/openmp mutex lock are used?
list_of_containers = list(set_of_containers)
list_of_containers.sort(reverse=False)
plt.figure(figsize = (12, 5))
base = [0] * len(list_of_containers)
for lock_type in container_lock_mutex_usage.keys():
    cur_bar = [container_lock_mutex_usage[lock_type].get(container_name, 0) for container_name in list_of_containers]
    random.shuffle(colors)
    plt.barh(list_of_containers, cur_bar, height=0.4, align='center', left=base)
    base = [base[i] + cur_bar[i] for i in range(len(base))]
plt.xticks(fontsize = 16)
plt.yticks(fontsize = 16)
plt.xlabel("total occurrence", fontsize=18)
# plt.ylabel("container name", fontsize=18)
plt.legend(container_lock_mutex_usage.keys(), fontsize=16)
plt.tight_layout()
plt.savefig(prefix + '/container_lock.png')
plt.close()

# how many containers are protected by mutex lock in percentage?
list_of_containers.sort(reverse=True)
plt.figure(figsize = (12, 5))
total_calls = [container_usage.get(container_name, 0) for container_name in list_of_containers] 
percentage = [base[i] * 100 / total_calls[i] for i in range(len(base))]
plt.bar([c.replace("std::", "") for c in list_of_containers], percentage, width=0.4, align='center', color='maroon')
plt.xticks(fontsize = 16)
plt.yticks(fontsize = 16)
# plt.xlabel("container name", fontsize=18)
plt.ylabel("percentage(%)", fontsize=18)
plt.savefig(prefix + '/container_lock_percentage.png')
plt.close()

