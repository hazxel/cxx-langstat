#include "RAJA/RAJA.hpp"

template <typename T>
T *allocate(std::size_t size)
{
  T *ptr;
  ptr = new T[size];
  return ptr;
}

template<typename T>
void nothing (T t) {}

void test() {}

int myfunc (int argc, char *argv[]) {
    constexpr int N = 1000000;

    int *a = allocate<int>(N);
    int *b = allocate<int>(N);
    int *c = allocate<int>(N);

    RAJA::forall<RAJA::seq_exec>(RAJA::TypedRangeSegment<int>(0, N), [=] RAJA_HOST_DEVICE (int i) { 
        a[i] = -i;
        b[i] = i;
    });

    nothing<int>(1);

    test();

    return 0;
}



