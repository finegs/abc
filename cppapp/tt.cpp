#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T> & v) {
  os << '[';
  char sep[3]{'\0', ' ', '\0'};
  for (const auto& o : v) {
    os << sep << o;
    sep[0] = ',';
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
  cout << "#1 : " << v1 << endl;
  sort(v1.begin(), v1.end(), [](int a, int b) { return a > b; });
  sort(v2.begin(), v2.end(), comp_int);
  sort(v3.begin(), v3.end());
  cout << "#v1 : " << v1 << endl;
  cout << "#v2 : " << v2 << endl;
  cout << "#v3 : " << v3 << endl;


  return 0;
}
