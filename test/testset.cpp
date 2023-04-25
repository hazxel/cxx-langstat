#include <set>
#include <iostream>

struct Dumm {
    int i;
    int j;
};

struct S {
    int i;
    int j;
    Dumm d;
};

int main() {

    S s = {1, 2};
    S t = {2, 3};

    auto cmp = [](const S &a, const S &b){ return a.i == b.i ? a.j > b.j : a.i > b.i; };

    std::set<S, decltype(cmp)> sett;

    std::cout << sett.size() << std::endl;
    sett.insert(s);
    sett.insert(t);

    S st = {1,2};

    sett.erase(st);

    for (auto s : sett) {
        std::cout << s.i << " " << s.j << std::endl;
    }

    return 0;
}