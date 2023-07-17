import os
import json
import random
import matplotlib.pyplot as plt

prefix = '/Users/boyan/Documents/ETH-Courses/Thesis/statistics-out'    

constructors = {}
methods = {}
functions = {}

lib_usage = {}

thread_usage = {}
parallel_usage = {}
lock_mutex_usage = {}
atomic_usage = {}
atomic_memroy_order_function_usage = {}

std_lib_usage = {}
boost_lib_usage = {}    
openmp_lib_usage = {}
tbb_lib_usage = {}
pthread_lib_usage = {}
mpi_lib_usage = {}
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
                lib_usage["STL"] = lib_usage.get("STL", 0) + calls
                project_lib_usage[projectname]["STL"] = project_lib_usage[projectname].get("STL", 0) + calls
                std_lib_usage["mutex&lock"] = std_lib_usage.get("mutex&lock", 0) + calls

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
                std_lib_usage["thread"] = std_lib_usage.get("thread", 0) + sum
            elif "mutex" in instance_name or "lock" in instance_name:
                lock_mutex_usage["STL"] = lock_mutex_usage.get("STL", 0) + sum
                std_lib_usage["mutex&lock"] = std_lib_usage.get("mutex&lock", 0) + sum
            elif "atomic" in instance_name:
                atomic_usage["STL"] = atomic_usage.get("STL", 0) + sum
                std_lib_usage["atomic"] = std_lib_usage.get("atomic", 0) + sum
        elif "boost::" in instance_name:
            lib_usage["Boost"] = lib_usage.get("Boost", 0) + sum
            project_lib_usage[projectname]["Boost"] = project_lib_usage[projectname].get("Boost", 0) + sum
            if "thread" in instance_name:
                thread_usage["Boost"] = thread_usage.get("Boost", 0) + sum
                boost_lib_usage["thread"] = boost_lib_usage.get("thread", 0) + sum
            elif "mutex" in instance_name or "lock" in instance_name:
                lock_mutex_usage["Boost"] = lock_mutex_usage.get("Boost", 0) + sum
                boost_lib_usage["mutex&lock"] = boost_lib_usage.get("mutex&lock", 0) + sum
            elif "atomic" in instance_name:
                atomic_usage["Boost"] = atomic_usage.get("Boost", 0) + sum
                boost_lib_usage["atomic"] = boost_lib_usage.get("atomic", 0) + sum
        elif "omp_" in instance_name:
            lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + sum
            project_lib_usage[projectname]["OpenMP"] = project_lib_usage[projectname].get("OpenMP", 0) + sum
            if "thread" in instance_name:
                thread_usage["OpenMP"] = thread_usage.get("OpenMP", 0) + sum
                openmp_lib_usage["thread"] = openmp_lib_usage.get("thread", 0) + sum
            elif "lock" in instance_name:
                lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + sum
                openmp_lib_usage["mutex&lock"] = openmp_lib_usage.get("mutex&lock", 0) + sum
        elif "tbb::" in instance_name:
            lib_usage["TBB"] = lib_usage.get("TBB", 0) + sum
            project_lib_usage[projectname]["TBB"] = project_lib_usage[projectname].get("TBB", 0) + sum
            if "mutex" in instance_name or "lock" in instance_name:
                lock_mutex_usage["TBB"] = lock_mutex_usage.get("TBB", 0) + sum
                tbb_lib_usage["mutex&lock"] = tbb_lib_usage.get("mutex&lock", 0) + sum
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
                std_lib_usage["parallel_algo"] = std_lib_usage.get("parallel_algo", 0) + calls
        elif "pthread_" in function_name:
            lib_usage["pthread"] = lib_usage.get("pthread", 0) + calls
            project_lib_usage[projectname]["pthread"] = project_lib_usage[projectname].get("pthread", 0) + calls
            if "mutex" in function_name:
                lock_mutex_usage["pthread"] = lock_mutex_usage.get("pthread", 0) + calls
                pthread_lib_usage["mutex&lock"] = pthread_lib_usage.get("mutex&lock", 0) + calls
            else:
                thread_usage["pthread"] = thread_usage.get("pthread", 0) + calls
                pthread_lib_usage["thread"] = pthread_lib_usage.get("thread", 0) + calls
        elif "MPI_" in function_name:
            lib_usage["MPI"] = lib_usage.get("MPI", 0) + calls
            project_lib_usage[projectname]["MPI"] = project_lib_usage[projectname].get("MPI", 0) + calls
            mpi_lib_usage[function_name] = mpi_lib_usage.get(function_name, 0) + calls
        elif "omp_" in function_name:
            lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + calls
            project_lib_usage[projectname]["OpenMP"] = project_lib_usage[projectname].get("OpenMP", 0) + calls
            # if "thread" in instance_name:
            #     thread_usage["OpenMP"] = thread_usage.get("OpenMP", 0) + sum
            if "lock" in instance_name:
                lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + sum
                openmp_lib_usage["mutex&lock"] = openmp_lib_usage.get("mutex&lock", 0) + sum
        elif "RAJA::" in function_name:
            lib_usage["RAJA"] = lib_usage.get("RAJA", 0) + calls
            project_lib_usage[projectname]["RAJA"] = project_lib_usage[projectname].get("RAJA", 0) + calls
            if "forall" in function_name:
                parallel_usage["RAJA"] = parallel_usage.get("RAJA", 0) + calls
                raja_lib_usage["parallel_algo"] = raja_lib_usage.get("parallel_algo", 0) + calls
            elif "atomic" in function_name:
                atomic_usage["RAJA"] = atomic_usage.get("RAJA", 0) + calls
                raja_lib_usage["atomic"] = raja_lib_usage.get("atomic", 0) + calls
            elif "mutex" in function_name:
                lock_mutex_usage["RAJA"] = lock_mutex_usage.get("RAJA", 0) + calls
                raja_lib_usage["mutex&lock"] = raja_lib_usage.get("mutex&lock", 0) + calls
            else:
                raja_lib_usage["other"] = raja_lib_usage.get("other", 0) + calls
        elif "Kokkos::" in function_name:
            lib_usage["Kokkos"] = lib_usage.get("Kokkos", 0) + calls
            project_lib_usage[projectname]["Kokkos"] = project_lib_usage[projectname].get("Kokkos", 0) + calls
            if "parallel_" in function_name:
                parallel_usage["Kokkos"] = parallel_usage.get("Kokkos", 0) + calls
                kokkos_lib_usage["parallel_algo"] = kokkos_lib_usage.get("parallel_algo", 0) + calls
            elif "atomic" in function_name:
                atomic_usage["Kokkos"] = atomic_usage.get("Kokkos", 0) + calls
                kokkos_lib_usage["atomic"] = kokkos_lib_usage.get("atomic", 0) + calls
            elif "lock" in function_name:
                lock_mutex_usage["Kokkos"] = lock_mutex_usage.get("Kokkos", 0) + calls
                kokkos_lib_usage["mutex&lock"] = kokkos_lib_usage.get("mutex&lock", 0) + calls
            else:
                kokkos_lib_usage[function_name] = kokkos_lib_usage.get(function_name, 0) + calls
        elif "tbb::" in function_name:
            lib_usage["TBB"] = lib_usage.get("TBB", 0) + calls
            project_lib_usage[projectname]["TBB"] = project_lib_usage[projectname].get("TBB", 0) + calls
            if "parallel_" in function_name:
                parallel_usage["TBB"] = parallel_usage.get("TBB", 0) + calls
                tbb_lib_usage["parallel_algo"] = tbb_lib_usage.get("parallel_algo", 0) + calls
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
        if "Parallel" in ompdir_name or "For" in ompdir_name:
            parallel_usage["OpenMP"] = parallel_usage.get("OpenMP", 0) + calls
            openmp_lib_usage["parallel_algo"] = openmp_lib_usage.get("parallel_algo", 0) + calls
        elif "Atomic" in ompdir_name:
            atomic_usage["OpenMP"] = atomic_usage.get("OpenMP", 0) + calls
            openmp_lib_usage["atomic"] = openmp_lib_usage.get("atomic", 0) + calls
        elif "Critical" in ompdir_name:
            lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + calls
            openmp_lib_usage["mutex&lock"] = openmp_lib_usage.get("mutex&lock", 0) + calls
        else:
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
            container_usage[container_name] = container_usage.get(container_name, 0) + calls
    f.close()

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
                elif "forward_list" in container_type_name:
                    container_type_name = "std::forward_list"
                elif "list" in container_type_name:
                    container_type_name = "std::list"
                elif "unordered_map" in container_type_name:
                    container_type_name = "std::unordered_map"
                elif "map" in container_type_name:
                    container_type_name = "std::map"
                elif "unordered_set" in container_type_name:
                    container_type_name = "std::unordered_set"
                elif "set" in container_type_name:
                    container_type_name = "std::set"
                elif "deque" in container_type_name:
                    container_type_name = "std::deque"
                elif "priority_queue" in container_type_name:
                    container_type_name = "std::priority_queue"
                elif "queue" in container_type_name:
                    container_type_name = "std::queue"
                elif "stack" in container_type_name:
                    container_type_name = "std::stack"

            set_of_containers.add(container_type_name)
            calls = 0
            for method_name, count in method_calls.items():
                calls += count
            container_lock_mutex_usage[mutex_lock_type][container_type_name] = container_lock_mutex_usage[mutex_lock_type].get(container_type_name, 0) + calls
    f.close()


########
# plot #
########


colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']
def plot_feature(
        feature_usage_dict, 
        fig_name,
        x_label_name = "total calls",
        y_label_name = ""):
    num_rows = len(feature_usage_dict)
    height = max(0.4, num_rows*0.6 + 2)
    plt.figure(figsize = (10, height))
    plt.ylim(bottom = -0.5, top = len(feature_usage_dict) - 0.5)
    plt.barh(list(feature_usage_dict.keys()), list(feature_usage_dict.values()), height=0.35, align='center', color='maroon')
    plt.xticks(fontsize = 12)
    plt.yticks(fontsize = 14)
    plt.xlabel(x_label_name, fontsize=16)
    plt.ylabel(y_label_name, fontsize=16)
    
    plt.tight_layout()
    plt.savefig(prefix + '/' + fig_name)
    plt.close()

plot_feature(lib_usage, "lib.png")
plot_feature(thread_usage, "thread.png")
plot_feature(lock_mutex_usage, "lock_mutex.png")
plot_feature(parallel_usage, "parallel.png")
plot_feature(atomic_usage, "atomic.png")

plot_feature(std_lib_usage, "std_lib.png", y_label_name = "primitives")
plot_feature(tbb_lib_usage, "tbb_lib.png", y_label_name = "primitives")
plot_feature(openmp_lib_usage, "omp_lib.png", y_label_name = "primitives")
plot_feature(pthread_lib_usage, "pthread_lib.png", y_label_name = "primitives")
plot_feature(mpi_lib_usage, "mpi_lib.png", y_label_name = "primitives")
plot_feature(kokkos_lib_usage, "kokkos_lib.png", y_label_name = "primitives")
plot_feature(raja_lib_usage, "raja_lib.png", y_label_name = "primitives")
plot_feature(container_usage, "container.png", y_label_name = "container type")

# statistics for atomic memory order
for function_name, memory_order_count_dict in atomic_memroy_order_function_usage.items():
    plot_feature(memory_order_count_dict, "atomic_memory_order_" + function_name + ".png", x_label_name = "total usage", y_label_name = "memory order")

# statistics for all the projects
plt.figure(figsize = (15, 11))
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
plt.xticks(fontsize = 16)
plt.xlabel("total calls", fontsize=18)
plt.yticks(fontsize = 18)
plt.legend(set_of_libs, fontsize=16)
plt.savefig(prefix + '/project_lib.png')
plt.close()

# statistics for mutex lock prevalence in container usage
### in what percentage of each STL container usage, STL/boost/tbb/pthread/openmp mutex lock are used?
plt.figure(figsize = (12, 5))
base = [0] * len(set_of_containers)
for lock_type in container_lock_mutex_usage.keys():
    cur_bar = [container_lock_mutex_usage[lock_type].get(container_name, 0) for container_name in set_of_containers]
    random.shuffle(colors)
    plt.barh(list(set_of_containers), cur_bar, height=0.4, align='center', left=base)
    base = [base[i] + cur_bar[i] for i in range(len(base))]
# plt.title("mutex lock usage in containers")
plt.xlabel("total calls")
plt.ylabel("container name")
plt.legend(container_lock_mutex_usage.keys())
plt.savefig(prefix + '/container_lock.png')
plt.close()

# how many containers are protected by mutex lock in percentage?
plt.figure(figsize = (12, 5))
total_calls = [container_usage.get(container_name, 0) for container_name in set_of_containers] 
percentage = [base[i] * 100 / total_calls[i] for i in range(len(base))]
plt.bar(list(set_of_containers), percentage, width=0.4, align='center', color='maroon')
# plt.title("percentage of containers protected by mutex/lock")
plt.xlabel("container name")
plt.ylabel("percentage(%)")
plt.savefig(prefix + '/container_lock_percentage.png')
plt.close()
