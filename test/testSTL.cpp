#include <algorithm>
#include <numeric>
#include <vector>
 
int main()
{
    std::vector<int> v {3, -4, 2, -8, 15, 267};
 
    auto do_nothing = [](const int& n) {};
 
    std::for_each(v.cbegin(), v.cend(), do_nothing);

    std::reduce(v.cbegin(), v.cend(), 0);

    std::accumulate(v.cbegin(), v.cend(), 0);
}