
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

auto item_hasher = [](const IntPair& o) { return std::hash<int>()(o.first); };
auto item_less = [](const IntPair& a, const IntPair& b) { return a.first < b.first; };
auto item_grater = [](const IntPair& a, const IntPair& b) { return a.first > b.first; };
auto item_equal_to = [](const IntPair& a, const IntPair& b) { return a.first == b.first; };

set<IntPair, decltype(item_less)> items{};

int main()
{
    int tc = 1;
    items.insert({11,1});
    items.insert({2,2});
    items.insert({1,2});

    // item display
    char sep[3]{'\0', ' ', '\0'};
    std::function<void(const IntPair& o)> item_display = [&sep](const auto& o) { 
                cout << sep << '{' << o.first << ',' << o.second << '}'; 
                sep[0]=','; 
    };   


    // #1 set
    cout << "#" << ++tc << std::endl;
    for_each(items.begin(), items.end(), item_display);
    cout << std::endl;

    // #2 set
    unordered_set<IntPair, decltype(item_hasher), decltype(item_equal_to)> hset(items.begin(), items.end());
    cout << "#" << ++tc << std::endl;
    sep[0]='\0';
    for_each(hset.begin(), hset.end(), item_display);
    cout << std::endl;
}

#endif
