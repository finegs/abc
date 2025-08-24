#include <bits/stdc++.h>
#include <iostream>
#include <queue>
#include <vector>

#if 1 // dijkstra

#include <climits>
using namespace std;

// Function to construct adjacency
vector<vector<vector<int>>> constructAdj(vector<vector<int>> &edges, int V) {

  // adj[u] = list of {v, wt}
  vector<vector<vector<int>>> adj(V);

  for (const auto &edge : edges) {
    int u = edge[0];
    int v = edge[1];
    int wt = edge[2];
    adj[u].push_back({v, wt});
    adj[v].push_back({u, wt});
  }

  return adj;
}

// Returns shortest distances from src to all other vertices
vector<int> dijkstra(int V, vector<vector<int>> &edges, int src) {

  // Create adjacency list
  vector<vector<vector<int>>> adj = constructAdj(edges, V);

  // Create a priority queue to store vertices that
  // are being preprocessed.
  priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> pq;

  // Create a vector for distances and initialize all
  // distances as infinite
  vector<int> dist(V, INT_MAX);

  // Insert source itself in priority queue and initialize
  // its distance as 0.
  pq.push({0, src});
  dist[src] = 0;

  // Looping till priority queue becomes empty (or all
  // distances are not finalized)
  while (!pq.empty()) {

    // The first vertex in pair is the minimum distance
    // vertex, extract it from priority queue.
    int u = pq.top()[1];
    pq.pop();

    // Get all adjacent of u.
    for (auto x : adj[u]) {

      // Get vertex label and weight of current
      // adjacent of u.
      int v = x[0];
      int weight = x[1];

      // If there is shorter path to v through u.
      if (dist[v] > dist[u] + weight) {
        // Updating distance of v
        dist[v] = dist[u] + weight;
        pq.push({dist[v], v});
      }
    }
  }

  return dist;
}

// Driver program to test methods of graph class
int main() {
  int V = 5;
  int src = 0;

  // edge list format: {u, v, weight}
  vector<vector<int>> edges = {{0, 1, 4}, // to avoid auto-indent 1-line
                               {0, 2, 8},
                               {1, 4, 6},
                               {2, 3, 2},
                               {3, 4, 10}};

  vector<int> result = dijkstra(V, edges, src);

  // Print shortest distances in one line
  for (int dist : result)
    cout << dist << " ";

  return 0;
}

#endif // 1
//
#if 0 // bellman_ford
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
