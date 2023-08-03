#include <omp.h>
#include <stdio.h>
#include <vector>

// #include <iostream>
// #include <unistd.h>


void dummy(std::vector<int>& v, int i){
	v[i] = 1;
}

int main() {
	#pragma omp parallel
	{
		int a = 0;
		#pragma omp critical // the next line is a critical section!
		{
			a++;
		}
		printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
	}

	#pragma omp parallel for
		for (int i = 0; i < 100; ++i) {
			printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
		}

	std::vector<int> v(100);
	#pragma omp critical
	{
		v[0] = 1;
	}

	#pragma omp critical
		dummy(v, v.size() - 1);

	// omp_lock_t lock;

	// 	omp_init_lock(&lock);

	// #pragma omp parallel num_threads(4)
	// {
	// 	omp_set_lock(&lock); // mutual exclusion (mutex)
	// 	std::cout << "Thread " << omp_get_thread_num() <<
	// 	    " has acquired the lock. Sleeping 2 seconds..." << std::endl;
	// 	sleep(2);
	// 	std::cout << "Thread " << omp_get_thread_num() <<
	// 	    " is releasing the lock..." << std:: endl;
	// 	omp_unset_lock(&lock);
	// }
	// 	omp_destroy_lock(&lock);
	// 	return 0;
	int aa = 0;
	#pragma omp atomic write
	aa = 1+1;
	

}