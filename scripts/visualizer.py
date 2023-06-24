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

project_lib_usage = {}

analysis = "cslma"
for filename in os.listdir(prefix + '/' + analysis):
    if not filename.endswith(".json"):
        continue

    projectname= filename.split('.')[0]
    print("Processing " + projectname + " with " + analysis + "...")
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
            elif "mutex" in instance_name or "lock" in instance_name:
                lock_mutex_usage["STL"] = lock_mutex_usage.get("STL", 0) + sum
            elif "atomic" in instance_name:
                atomic_usage["STL"] = atomic_usage.get("STL", 0) + sum
        elif "boost::" in instance_name:
            lib_usage["Boost"] = lib_usage.get("Boost", 0) + sum
            project_lib_usage[projectname]["Boost"] = project_lib_usage[projectname].get("Boost", 0) + sum
            if "thread" in instance_name:
                thread_usage["Boost"] = thread_usage.get("Boost", 0) + sum
            elif "mutex" in instance_name or "lock" in instance_name:
                lock_mutex_usage["Boost"] = lock_mutex_usage.get("Boost", 0) + sum
            elif "atomic" in instance_name:
                atomic_usage["Boost"] = atomic_usage.get("Boost", 0) + sum
        elif "omp_" in instance_name:
            lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + sum
            project_lib_usage[projectname]["OpenMP"] = project_lib_usage[projectname].get("OpenMP", 0) + sum
            if "thread" in instance_name:
                thread_usage["OpenMP"] = thread_usage.get("OpenMP", 0) + sum
            elif "lock" in instance_name:
                lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + sum
        elif "tbb::" in instance_name:
            lib_usage["TBB"] = lib_usage.get("TBB", 0) + sum
            project_lib_usage[projectname]["TBB"] = project_lib_usage[projectname].get("TBB", 0) + sum
            if "mutex" in instance_name:
                lock_mutex_usage["TBB"] = lock_mutex_usage.get("TBB", 0) + sum
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
        elif "pthread_" in function_name:
            lib_usage["pthread"] = lib_usage.get("pthread", 0) + calls
            project_lib_usage[projectname]["pthread"] = project_lib_usage[projectname].get("pthread", 0) + calls
            thread_usage["pthread"] = thread_usage.get("pthread", 0) + calls
        elif "MPI_" in function_name:
            lib_usage["MPI"] = lib_usage.get("MPI", 0) + calls
            project_lib_usage[projectname]["MPI"] = project_lib_usage[projectname].get("MPI", 0) + calls
        elif "omp_" in function_name:
            lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + calls
            project_lib_usage[projectname]["OpenMP"] = project_lib_usage[projectname].get("OpenMP", 0) + calls
            # if "thread" in instance_name:
            #     thread_usage["OpenMP"] = thread_usage.get("OpenMP", 0) + sum
            if "lock" in instance_name:
                lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + sum
        elif "RAJA::" in function_name:
            lib_usage["RAJA"] = lib_usage.get("RAJA", 0) + calls
            project_lib_usage[projectname]["RAJA"] = project_lib_usage[projectname].get("RAJA", 0) + calls
            if "forall" in function_name:
                parallel_usage["RAJA"] = parallel_usage.get("RAJA", 0) + calls
        elif "Kokkos::" in function_name:
            lib_usage["Kokkos"] = lib_usage.get("Kokkos", 0) + calls
            project_lib_usage[projectname]["Kokkos"] = project_lib_usage[projectname].get("Kokkos", 0) + calls
            if "parallel_" in function_name:
                parallel_usage["Kokkos"] = parallel_usage.get("Kokkos", 0) + calls
        elif "tbb::" in function_name:
            lib_usage["TBB"] = lib_usage.get("TBB", 0) + calls
            project_lib_usage[projectname]["TBB"] = project_lib_usage[projectname].get("TBB", 0) + calls
            if "parallel_" in function_name:
                parallel_usage["TBB"] = parallel_usage.get("TBB", 0) + calls   
        else:
            lib_usage["other"] = lib_usage.get("other", 0) + calls
            project_lib_usage[projectname]["other"] = project_lib_usage[projectname].get("other", 0) + calls
            print(projectname, function_name)
    f.close()

analysis = "ompeda"
for filename in os.listdir(prefix + '/' + analysis):
    if not filename.endswith(".json"):
        continue

    projectname= filename.split('.')[0]
    project_lib_usage[projectname] = project_lib_usage.get(projectname, {})

    f = open(prefix + '/' + analysis + '/' + filename, 'r')
    data = json.load(f)

    for ompdir_name, calls in data["Summary"]["omp_directive"].items():
        if "Parallel" in ompdir_name or "For" in ompdir_name:
            parallel_usage["OpenMP"] = parallel_usage.get("OpenMP", 0) + calls
        elif "Atomic" in ompdir_name:
            atomic_usage["OpenMP"] = atomic_usage.get("OpenMP", 0) + calls
        elif "Critical" in ompdir_name:
            lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + calls
        else:
            continue
        lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + calls
        project_lib_usage[projectname]["OpenMP"] = project_lib_usage[projectname].get("OpenMP", 0) + calls

    f.close()

keys = list(project_lib_usage.keys())
keys.sort(reverse=True)
project_lib_usage = {k: project_lib_usage[k] for k in keys}
# print(constructors)
# print(methods)
# print(functions)
# print(lib_usage)
# print(thread_usage)
# print(lock_mutex_usage)
# print(project_lib_usage)


colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']

plt.figure(figsize = (10, 5))
plt.barh(list(lib_usage.keys()), list(lib_usage.values()), height=0.4, align='center', color='maroon')
plt.title("lib usage")
plt.xlabel("total calls")
plt.ylabel("libraries")
# plt.show()
plt.savefig(prefix + '/lib.png')

plt.figure(figsize = (10, 5))
plt.barh(list(thread_usage.keys()), list(thread_usage.values()), height=0.4, align='center', color='maroon')
plt.title("thread usage")
plt.xlabel("total calls")
plt.ylabel("libraries")
# plt.show()
plt.savefig(prefix + '/thread.png')

plt.figure(figsize = (10, 5))
plt.barh(list(lock_mutex_usage.keys()), list(lock_mutex_usage.values()), height=0.4, align='center', color='maroon')
plt.title("lock/mutex usage")
plt.xlabel("total calls")
plt.ylabel("libraries")
# plt.show()
plt.savefig(prefix + '/lock_mutex.png')

plt.figure(figsize = (10, 5))
plt.barh(list(parallel_usage.keys()), list(parallel_usage.values()), height=0.4, align='center', color='maroon')
plt.title("parallel execution usage")
plt.xlabel("total calls")
plt.ylabel("libraries")
# plt.show()
plt.savefig(prefix + '/parallel.png')

plt.figure(figsize = (12, 5))
set_of_libs = set()
for lib_usage in project_lib_usage.values():
    for lib in lib_usage.keys():
        set_of_libs.add(lib)
base=[0] * len(project_lib_usage)
for lib in set_of_libs:
    cur_bar = [lib_usage.get(lib, 0) for lib_usage in project_lib_usage.values()]
    random.shuffle(colors)
    plt.barh(list(project_lib_usage.keys()), cur_bar, height=0.4, align='center', left=base, color = colors.pop())
    base = [base[i] + cur_bar[i] for i in range(len(base))] 
plt.title("concurrency support libraries usage of HPC projects")
plt.xlabel("total number of calls")
plt.ylabel("project name")
plt.legend(set_of_libs)
# plt.show()
plt.savefig(prefix + '/project_lib.png')
