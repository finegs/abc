
#if 1

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

struct Log {
  int id, x1, x2, y;

  bool operator<(const Log& o) const {
    if (x2 == o.x2) return x1 < o.x1;
    return x2 > o.x2; 
  }

  friend std::ostream& operator<<(std::ostream& os, const Log& o) {
    os << "{";
    os << "id:" << o.id;
    os << "," << "x1:" << o.x1;
    os << "," << "x2:" << o.x2;
    os << "," << "y:" << o.y;
    os << "}";
    return os;
  }
}; 

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T>& v) {
  char sep[3]{'\0', ',', '\0'};
  os << "len : " << v.size();
  os << "," << "contents : [";
  for (const auto& o: v) {
    os << sep << o;
    sep[0]=',';
  }
  os << "]";
  return os;
}

int main(int argc, char *argv[])
{
  int N,K;
  cin >> N >> K;

  unordered_map<int, Log> logs;
  vector<Log> logss(1000010);
  Log log;
  logss.resize(100010);
  for (int i = 0 ; i < N;i++) {
      cin >> log.id >> log.x1>> log.x2>> log.y; 
      logss[i] = log;
      logs.insert({log.id, log});
  }

  char sep[]{'\0', ' ', '\0'};
  std::cout << logss << endl;
  return 0;
}

#endif
