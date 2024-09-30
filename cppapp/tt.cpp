#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template<typename It>
std::ostream& display(std::ostream& os, It begin, It end) {
  os << '[';
  char sep[3]{'\0', ' ', '\0'};
  for (auto& cur=begin;cur!=end;++cur) {
    os << sep << *cur; sep[0] = ',';
  }
  os << ']';
  return os;
}

bool comp_int(int a, int b) {
  return a < b;
}

int main() {

  vector<int> v1{111, 22 ,33, 13421,1,2,3};
  vector<int> v2 = v1;
  vector<int> v3 = v1;

  cout << "#1 : "; display(cout, v1.begin(), v1.end()) << endl;
  sort(v1.begin(), v1.end(), [](int a, int b) { return a > b; });
  sort(v2.begin(), v2.end(), comp_int);
  sort(v3.begin(), v3.end());

  cout << "#v1 : "; display(cout, v1.begin(), v1.end()) << endl;
  cout << "#v2 : "; display(cout, v2.begin(), v2.end()) << endl;
  cout << "#v3 : "; display(cout, v3.begin(), v3.end()) << endl;


  return 0;
}
