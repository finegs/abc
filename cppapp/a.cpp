
#include <iomanip>
#include <iostream>
#include <utility>
#if 1

#include <algorithm>
#include <cstdio>
#include <functional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using IntPair = std::pair<int, int>;
using MyRec = std::tuple<int, float, string>;

std::ostream &operator<<(std::ostream &os, const IntPair &o) {
  os << '{';
  os << o.first;
  os << ':';
  os << std::setw(10);
  os << o.second;
  os << '}';
  return os;
}

std::ostream &operator<<(std::ostream &os, const MyRec &o) {
  os << '{';
  os << std::setw(3);
  os << std::get<0>(o);
  os << ", ";
  os << std::setw(10);
  os << std::get<1>(o);
  os << ", ";
  os << std::get<2>(o);
  os << '}';
  return os;
}

template <typename first, typename second>
std::ostream &operator<<(std::ostream &os, const std::pair<first, second> &o) {
  os << '{';
  os << '[' << (&o.first) << ']' << o.first;
  os << ':' << '[' << (&o.second) << ']' << o.second;
  os << '}';
  return os;
}
template <typename It>
std::ostream &display(std::ostream &os, It begin, It end) {
  bool is_large = (std::distance(begin, end) > 20) ? true : false;
  char sep[3]{'\0', ' ', '\0'};
  if (is_large) {
    sep[0] = '\0';
    //
    //  first 3
    auto nxt3_iter = std::next(begin, 3);

    for (auto &cur = begin; cur != nxt3_iter; ++cur) {
      os << sep << '\t' << *cur;
      sep[0] = '\n';
    }
    os << std::endl;

    os << "..." << std::endl;
    //  last 3
    auto pr3_iter = std::next(end, -3);

    sep[0] = '\0';
    for (auto &cur = pr3_iter; cur != end; ++cur) {
      os << sep << '\t' << *cur;
      sep[0] = '\n';
    }
  } else {
    sep[0] = '\0';
    for (auto &cur = begin; cur != end; ++cur) {
      os << sep << '\t' << *cur;
      sep[0] = '\n';
    }
  }

  return os;
}

auto item_hasher = [](const IntPair &o) { return std::hash<int>{}(o.first); };
auto item_less = [](const IntPair &a, const IntPair &b) {
  return a.first < b.first;
};
auto item_grater = [](const IntPair &a, const IntPair &b) {
  return a.first > b.first;
};
auto item_equal_to = [](const IntPair &a, const IntPair &b) {
  return a.first == b.first;
};

auto myrec_hasher = [](const MyRec &o) {
  return std::hash<int>{}(std::get<0>(o));
};
auto myrec_equal_to = [](const MyRec &a, const MyRec &b) {
  return std::get<0>(a) == std::get<0>(b) && std::get<1>(a) == std::get<1>(b) &&
         std::get<2>(a) == std::get<2>(b);
};

/*
 *
 * struct hash_tuple {
    template <std::size_t...Index>
    size_t recursive_hash(const auto &x) const{
        return (boost::get<Index>(x) ^ ... );
    }

    template <template <typename> class Ts,typename...Args>
    size_t operator()(const Ts<Args...>& x) const{
        return
recursive_hash<std::make_integer_sequence<int,sizeof...(Args)>>(x);
    }
};

unordered_set<IntPair,  decltype(item_hasher)>  item_unordered_set{};
using Map = std::unordered_map<Key, int, hash_tuple>;

// define a hash function for this tuple
struct KeyHash : public std::unary_function<MyTuple, std::size_t> {
    std::size_t operator()(const MyTuple& k) const {
        // the magic operation below makes collisions less likely than just the
standard XOR std::size_t seed = std::hash<int>{}(std::get<0>(k)); seed ^=
std::hash<char>{}(std::get<1>(k)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed ^ (std::hash<char>{}(std::get<2>(k)) + 0x9e3779b9 + (seed <<
6) + (seed >> 2));
    }
};

// define the comparison operator for this tuple
struct KeyEqual : public std::binary_function<MyTuple, MyTuple, bool> {
    bool operator()(const MyTuple& v0, const MyTuple& v1) const {
        return (std::get<0>(v0) == std::get<0>(v1) && std::get<1>(v0) ==
std::get<1>(v1) && std::get<2>(v0) == std::get<2>(v1));
    }
};

typedef unordered_map<MyTuple, int, KeyHash, KeyEqual> MyMap;
 */
std::set<IntPair, decltype(item_less)> item_set{};
std::unordered_set<IntPair, decltype(item_hasher)> item_unordered_set{};
std::unordered_map<MyRec, MyRec, decltype(myrec_hasher)> recs{
    {{1, 1.0, "aa"}, {1, 1.0001, "aa"}},
    {{2, 2.0, "bb"}, {2, 2.0101, "bb"}},
    {{1, 11.0, "11111"}, {2, 11.0001, "11111"}},
};

template <typename T>
std::vector<T> fill(std::vector<T> &v, size_t count,
                    std::function<size_t(size_t)> g) {
  v.resize(count);
  for (size_t i = 0; i < count; i++)
    v[i] = g(i);
  return v;
}

void init_item_set(set<IntPair, decltype(item_less)> &s, size_t count) {
  for (size_t i = 0; i < count; i++) {
    s.insert({i, i});
  }
}

void init_item_unorderd_set(unordered_set<IntPair, decltype(item_hasher)> &s,
                            size_t count) {
  for (size_t i = 0; i < count; i++) {
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

int main() {
  int tc = 0;

  cout << setprecision(8);

  // display #
  cout << "#" << ++tc << ':';
  display(std::cout, item_set.begin(), item_set.end());
  cout << std::endl;

  MyRec new_key = std::make_tuple(1, 1.0, "aa");
  MyRec new_val = new_key;
  std::get<1>(new_val) = 1.002002;

  cout << "## rect.insert_or_assign" << endl;
  // cout << "hash(1) : " << std::hash<int>{}(1) << std::endl;
  cout << "recs.hash_function(" << new_key << ")" << " : "
       << recs.hash_function()(new_key) << std::endl;

  cout << "## before\trect.insert_or_assign(" << new_key << ", " << new_val
       << ") : recs[" << new_key << "] = " << recs[new_key] << std::endl;
  recs.insert_or_assign(new_key, new_val);
  cout << "## after\trect.insert_or_assign(" << new_key << ", " << new_val
       << ") : recs[" << new_key << "] = " << recs[new_key] << std::endl;

  // display #
  cout << "#" << ++tc << ':';
  display(std::cout, recs.begin(), recs.end());
  cout << std::endl;

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
