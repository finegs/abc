
#include <utility>
#include <iostream>
#if 1

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <set>
#include <unordered_set>
#include <functional>

using namespace std;
using IntPair = std::pair<int, int>;

std::ostream& operator<<(std::ostream& os, const IntPair& o) {
    os << '{' << o.first << ':' << o.second << '}';
    return os;
}

template<typename It>
std::ostream& display(std::ostream& os, It begin, It end) {
    char sep[3]{'\0', ' ', '\0'};
    for(auto& cur = begin;cur != end; ++cur) {
       os << sep << *cur;  sep[0]=',';
    }
    return os;
}

auto item_hasher = [](const IntPair& o) { return std::hash<int>()(o.first); };
auto item_less = [](const IntPair& a, const IntPair& b) { return a.first < b.first; };
auto item_grater = [](const IntPair& a, const IntPair& b) { return a.first > b.first; };
auto item_equal_to = [](const IntPair& a, const IntPair& b) { return a.first == b.first; };

set<IntPair, decltype(item_less)> items{};

int main()
{
    int tc = 0;
    // init set
    items.insert({11,1});
    items.insert({2,2});
    items.insert({1,2});

    // display set #1
    cout << "#" << ++tc <<  ':'; display(std::cout, items.begin(), items.end());
    cout << std::endl;

    // init ordered_set
    unordered_set<IntPair, decltype(item_hasher), decltype(item_equal_to)> hset(items.begin(), items.end());

    // display unordered_set #2
    cout << "#" << ++tc << ':'; display(std::cout, hset.begin(), hset.end());
    cout << std::endl;
}

#endif
