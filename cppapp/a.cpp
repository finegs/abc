
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
auto myrec_equal_to = [](const MyRec& a, const MyRec& b) { return std::get<0>(a) == std::get<0>(b)
                                                               && std::get<1>(a) == std::get<1>(b)
                                                               && std::get<2>(a) == std::get<2>(b); };

/*
 *
 * struct hash_tuple {
    template <std::size_t...Index>
    size_t recursive_hash(const auto &x) const{
        return (boost::get<Index>(x) ^ ... );
    }

    template <template <typename> class Ts,typename...Args>
    size_t operator()(const Ts<Args...>& x) const{
        return recursive_hash<std::make_integer_sequence<int,sizeof...(Args)>>(x);
    }
};

unordered_set<IntPair,  decltype(item_hasher)>  item_unordered_set{};
using Map = std::unordered_map<Key, int, hash_tuple>;

// define a hash function for this tuple
struct KeyHash : public std::unary_function<MyTuple, std::size_t> {
    std::size_t operator()(const MyTuple& k) const {
        // the magic operation below makes collisions less likely than just the standard XOR
        std::size_t seed = std::hash<int>()(std::get<0>(k));
        seed ^= std::hash<char>()(std::get<1>(k)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed ^ (std::hash<char>()(std::get<2>(k)) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    }
};

// define the comparison operator for this tuple
struct KeyEqual : public std::binary_function<MyTuple, MyTuple, bool> {
    bool operator()(const MyTuple& v0, const MyTuple& v1) const {
        return (std::get<0>(v0) == std::get<0>(v1) && std::get<1>(v0) == std::get<1>(v1) &&
                std::get<2>(v0) == std::get<2>(v1));
    }
};

typedef unordered_map<MyTuple, int, KeyHash, KeyEqual> MyMap;
 */
std::set<IntPair, decltype(item_less)> item_set{};
std::unordered_set<IntPair,  decltype(item_hasher)>  item_unordered_set{};
std::unordered_map<MyRec, MyRec, decltype(myrec_hasher)> recs{
    {{1, 1.0, "aa"}, {1, 1.0001, "aa"}},
    {{2, 2.0, "bb"}, {2, 2.0101, "bb"}},
    {{1, 11.0, "11111"}, {2, 11.0001, "11111"}},
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
typedef struct _MyInitiator {
    _MyInitiator() {
        init_item_set(item_set, 1000);
        init_item_unorderd_set(item_unordered_set, 1000);
    }
} MyInitiator;

MyInitiator myInitiator;

int main()
{
    int tc = 0;
    
    // display #
    cout << "#" << ++tc << ':'; display(std::cout, item_set.begin(), item_set.end()); cout << std::endl;

    MyRec rr = std::make_tuple(1, 1.0, "aa");
    cout << "hash(1) : " << std::hash<int>()(1) << std::endl;
    cout << "recs[" << rr << " : " << recs[rr] << std::endl;
    cout << "hash(" << std::get<0>(rr) << ") : " << std::hash<int>()(std::get<0>(recs[rr])) << std::endl;

    auto e1 = std::make_tuple(1, 1.0, "aa");
    recs.insert_or_assign(e1, e1);

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
