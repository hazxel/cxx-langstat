import os
import json
import matplotlib.pyplot as plt

prefix = '/Users/boyan/Documents/ETH-Courses/Thesis/statistics-out'    

constructors = {}
methods = {}
functioins = {}

lib_usage = {}
thread_usage = {}
lock_mutex_usage = {}

for filename in os.listdir(prefix):
    if filename.endswith(".json"):
        print(filename)
        f = open(prefix + '/' + filename, 'r')
        data = json.load(f)


        for constructor_name, calls in data["Summary"]["constructor calls"].items():
            constructors[str(constructor_name)] = constructors.get(constructor_name, 0) + calls

        for instance_name, calls in data["Summary"]["method calls"].items():
            sum = 0
            for method_name, call in calls.items():
                if instance_name not in methods:
                    methods[instance_name] = {}
                methods[instance_name][method_name] = methods.get(instance_name, {}).get(method_name, 0) + call
                sum += call
            
            if "std::" in instance_name:
                lib_usage["STL"] = lib_usage.get("STL", 0) + sum
                if "thread" in instance_name:
                    thread_usage["STL"] = thread_usage.get("STL", 0) + sum
                if "mutex" in instance_name or "lock" in instance_name:
                    lock_mutex_usage["STL"] = lock_mutex_usage.get("STL", 0) + sum
            elif "boost::" in instance_name:
                lib_usage["Boost"] = lib_usage.get("Boost", 0) + sum
                if "thread" in instance_name:
                    thread_usage["Boost"] = thread_usage.get("Boost", 0) + sum
                if "mutex" in instance_name or "lock" in instance_name:
                    lock_mutex_usage["Boost"] = lock_mutex_usage.get("Boost", 0) + sum
            elif "omp_" in instance_name:
                lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + sum
                if "THREAD" in instance_name:
                    thread_usage["OpenMP"] = thread_usage.get("OpenMP", 0) + sum
                if "LOCK" in instance_name:
                    lock_mutex_usage["OpenMP"] = lock_mutex_usage.get("OpenMP", 0) + sum
            else:
                lib_usage["other"] = lib_usage.get("other", 0) + sum

            

        for function_name, calls in data["Summary"]["function calls"].items():
            functioins[function_name] = functioins.get(function_name, 0) + calls
            if "pthread_" in function_name:
                lib_usage["pthread"] = lib_usage.get("pthread", 0) + calls
                thread_usage["pthread"] = thread_usage.get("pthread", 0) + calls
            elif "MPI_" in function_name:
                lib_usage["MPI"] = lib_usage.get("MPI", 0) + calls
            elif "omp_" in function_name:
                lib_usage["OpenMP"] = lib_usage.get("OpenMP", 0) + calls
            else:
                lib_usage["other"] = lib_usage.get("other", 0) + calls

        f.close()
        continue
    else:
        continue


print(constructors)
print(methods)
print(functioins)
print(lib_usage)
print(thread_usage)
print(lock_mutex_usage)



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