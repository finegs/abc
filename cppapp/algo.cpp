#include <bits/stdc++.h>
#include <vector>
using namespace std;

#if 1
#include <climits>
using namespace std;

// #define INF 987654321
#define INF (INT_MAX - 1)

int bellman_ford(vector<pair<pair<int, int>, int>> &edges, int dist[],
                 int startNode, int nodeNum) {
  // 음수 사이클 시 -1, 아니면 0 리턴

  // 초기값 갱신
  dist[startNode] = 0;

  // 모든 노드 수만큼 모든 엣지에 대해 업데이트 체크
  for (int i = 0; i < nodeNum; i++) {
    for (auto e : edges) {
      // from -> to dist = d
      int from = e.first.first;
      int to = e.first.second;
      int d = e.second;

      // start -> from 가는 경로 없음
      if (dist[from] == INF)
        continue;

      // start -> to > (start -> from, from -> to) 인 경우 업데이트
      if (dist[to] > dist[from] + d) {
        dist[to] = dist[from] + d;

        // 전체 노드 수만큼 loop에도 업데이트되는 경우 = 음수 사이클 존재
        if (i == (nodeNum - 1))
          return -1;
      }
    }
  }

  return 0;
}

int main() {
  int nodeNum = 6;
  int graph[6][6] = {{0, 3, 2, -2, INF, INF},    {INF, 0, INF, INF, 5, INF},
                     {INF, 1, 0, -6, -3, 1},     {INF, INF, INF, 0, INF, 2},
                     {INF, INF, INF, INF, 0, 4}, {INF, INF, INF, INF, INF, 0}};

  vector<pair<pair<int, int>, int>> edges;
  for (int i = 0; i < nodeNum; i++) {
    for (int j = 0; j < nodeNum; j++) {
      if (i != j && graph[i][j] != INF) {
        edges.push_back({{i, j}, graph[i][j]});
      }
    }
  }

  for (int startNode = 0; startNode < nodeNum; startNode++) {
    int *dist = new int[nodeNum];
    for (int i = 0; i < nodeNum; i++) {
      dist[i] = INF;
    }

    int res = bellman_ford(edges, dist, startNode, nodeNum);

    if (res == -1) {
      cout << "negative cycle exist!";
      break;
    } else {
      cout << "start node " << startNode << " : ";
      for (int i = 0; i < nodeNum; i++) {
        if (dist[i] == INF)
          cout << "." << " ";
        else
          cout << dist[i] << " ";
      }
      cout << "\n";
    }

    delete[] dist;
  }
}

#endif
#if 0
int knapsackRec(int W, vector<int> &val, vector<int> &wt, int n) {

  if (n == 0 || W == 0)
    return 0;

  int pick = 0;

  if (wt[n - 1] <= W)
    pick = val[n - 1] + knapsackRec(W - wt[n - 1], val, wt, n - 1);

  int notPick = knapsackRec(W, val, wt, n - 1);

  return max(pick, notPick);
}

int knapsack(int W, vector<int> &val, vector<int> &wt) {
  int n = val.size();

  return knapsackRec(W, val, wt, n);
}

int main(int argc, char *argv[]) {
  vector<int> val = {1, 2, 3};
  vector<int> wt = {5, 5, 1};

  int W = 4;

  cout << knapsack(W, val, wt) << endl;
  return 0;
}

#endif
