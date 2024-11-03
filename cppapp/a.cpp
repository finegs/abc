
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
#include <tuple>

using namespace std;
using IntPair = std::pair<int, int>;
using MyRec = std::tuple<int, float, string>;

std::ostream& operator<<(std::ostream& os, const IntPair& o) {
    os << '{' << o.first << ':' << o.second << '}';
    return os;
}

std::ostream& operator<<(std::ostream& os, const MyRec& o) {
    os << '{' << std::get<0>(o) << ", " << std::get<1>(o) << ", " << std::get<2>(o) << '}';
    return os;
}

template<typename first, typename second>
std::ostream& operator<<(std::ostream& os, const std::pair<first, second>& o) {
    os << '{' << '[' << (&o.first) << ']' << o.first << ':' << '[' << (&o.first) << ']' <<o.second << '}';
    return os;
}
template<typename It>
std::ostream& display(std::ostream& os, It begin, It end) {
    bool  is_large =  (std::distance(begin, end )  >  20) ?  true : false;
    char sep[3]{'\0', ' ', '\0'};
    if  (is_large) {
        sep[0] = '\0';
        //
        //  first 3
        auto nxt3_iter = std::next(begin, 3);

        for(auto& cur = begin;cur != nxt3_iter; ++cur) {
            os << sep << *cur;  sep[0]='\n';
        }
        os <<  std::endl;

        os << "..." << std::endl;
        //  last 3 
        auto pr3_iter  = std::next(end, -3);

        sep[0] = '\0';
        for(auto& cur = pr3_iter;cur != end; ++cur) {
            os << sep << *cur;  sep[0]='\n';
        }
    }
    else {
        sep[0] = '\0';
        for(auto& cur = begin;cur != end; ++cur) {
           os << sep << *cur;  sep[0]='\n';
        }
    }

    return os;
}

auto item_hasher = [](const IntPair& o) { return std::hash<int>()(o.first); };
auto item_less = [](const IntPair& a, const IntPair& b) { return a.first < b.first; };
auto item_grater = [](const IntPair& a, const IntPair& b) { return a.first > b.first; };
auto item_equal_to = [](const IntPair& a, const IntPair& b) { return a.first == b.first; };

auto myrec_hasher = [](const MyRec& o) { return std::hash<int>()( std::get<0>(o) ); };

set<IntPair, decltype(item_less)> item_set{};
unordered_set<IntPair,  decltype(item_hasher)>  item_unordered_set{};
unordered_map<MyRec, MyRec, decltype(myrec_hasher)> recs{
    {{1, 1.0, "aa"}, {1, 10.1, "aa's desc"}},
    {{2, 2.0, "bb"}, {2, 20.1, "bb's desc"}},
    {{1, 11.0, "11111"}, {2, 20.1, "111111"}},
};

template<typename T>
std::vector<T> fill(std::vector<T>&  v, size_t  count, std::function<size_t(size_t)> g) {
    v.resize(count);
    for (size_t i = 0;i<count;i++) v[i] = g(i);
    return v;
}

void init_item_set(set<IntPair,  decltype(item_less)>& s, size_t count) {
    for (size_t i = 0 ; i< count; i++) {
        s.insert({i, i}); 
    }
}

void init_item_unorderd_set(unordered_set<IntPair,  decltype(item_hasher)>& s, size_t count) {
    for (size_t i = 0 ; i< count; i++) {
        s.insert({i, i}); 
    }
}
struct _MyInitiator {
    _MyInitiator() {
        init_item_set(item_set, 1000);
        init_item_unorderd_set(item_unordered_set, 1000);
    }
} MyInitiator;

_MyInitiator myInitiator;

int main()
{
    int tc = 0;
    
    // display #
    cout << "#" << ++tc << ':'; display(std::cout, item_set.begin(), item_set.end()); cout << std::endl;

    auto e1 = std::make_tuple(1, 1.0, "a is changed");
    recs.insert({e1, e1});

    // display #
    cout << "#" << ++tc << ':'; display(std::cout, recs.begin(), recs.end()); cout << std::endl;

#if 0
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
#endif
}

#endif
