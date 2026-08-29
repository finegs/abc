#include <algorithm>
#include <array>

#define N 10000

std::array<int, N> parent{0};
std::array<int, N> rank{0};

int find_set(int i) {
  if (parent[i] == i)
    return i;

  return parent[i] = find_set(parent[i]);
}

int union_set(int a, int b) {
  a = find_set(a);
  b = find_set(b);
  if (a == b)
    return a;

  if (rank[b] > rank[a]) {
    std::swap(a, b);
  }

  parent[b] = a;

  if (rank[a] != rank[b])
    rank[a] += 1;

  return a;
}

int main(int argc, char *argv[]) { return 0; }
