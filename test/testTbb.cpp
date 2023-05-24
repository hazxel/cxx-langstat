#include <tbb/mutex.h>
int          count;
tbb::mutex   countMutex;

int IncrementCount() {
    int result;
    {
       // Add oneTBB scoped lock at location of ANNOTATE_LOCK annotations
       tbb::mutex::scoped_lock lock(countMutex);  // Implements ANNOTATE_LOCK_ACQUIRE()
       result = count++;
	      // Implicit ANNOTATE_LOCK_RELEASE() when leaving the scope below. 
    }  // scoped lock is automatically released here
    return result;
}

int main(){
    IncrementCount();
}