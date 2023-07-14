#include <algorithm>
#include <numeric>
#include <vector>
#include <atomic>
#include <mutex>
#include <string>
 
int main()
{
    std::vector<int> v {3, -4, 2, -8, 15, 267};
 
    auto do_nothing = [](const int& n) {};
 
    std::for_each(v.cbegin(), v.cend(), do_nothing);

    std::reduce(v.cbegin(), v.cend(), 0);

    std::accumulate(v.cbegin(), v.cend(), 0);

    int a = 1;
    int b = 2;
    std::atomic_int acnt;
    acnt.fetch_add(1, std::memory_order_relaxed);
    acnt.compare_exchange_weak(a, b, std::memory_order_relaxed, std::memory_order_relaxed);
    acnt.load();

    std::atomic<int> aacnt;
    aacnt.load();
    aacnt.compare_exchange_strong(a, b, std::memory_order_relaxed, std::memory_order_relaxed);
    aacnt++;

    {
        {
            v.push_back(1);
            std::mutex m;
            v.push_back(2);
            std::lock_guard<std::mutex> lg(m);
            v.push_back(3);
            std::sort(v.begin(), v.end());
            v[0] = 1;
        }
    }
    std::string s;
    s.append("a");
    s[0] = 'b';
}