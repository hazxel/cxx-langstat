#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <utility>

using std::thread;
using std::mutex;
using std::cout;
using std::endl;

void set_one(int &n) {
    for (int i = 0; i < 100; ++i) {
        n =  1;
    }
}

void set_two(int &n) {
    for (int i = 0; i < 100; ++i) {
        n = 2;
    }
}

int main () {
    std::mutex m;
    int n = 0;

    std::pair<int,int> p = {1.0, 2.0};
    std::vector<int> v = {1,2,3,4,5};

    thread t1(set_one, std::ref(n));
    thread t2(set_two, std::ref(n));
    
    t1.join();
    t2.join();

    {
        std::lock_guard<mutex> lock(m);
        cout << n << endl;
    }

    {
        std::unique_lock<mutex> lock(m);
        cout << n << endl;
    }

    return 0;
}