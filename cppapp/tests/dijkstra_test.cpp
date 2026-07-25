#include <gtest/gtest.h>
#include <vector>
#include <queue>
#include <limits>

using namespace std;

constexpr int INF = 99999999;

vector<int> run_dijkstra(const vector<vector<pair<int, int>>> &graph, int start) {
  int n = static_cast<int>(graph.size());
  vector<int> dist(n, INF);
  priority_queue<pair<int, int>> pq;

  dist[start] = 0;
  pq.push({0, start});

  while (!pq.empty()) {
    int curDist = -pq.top().first;
    int curNode = pq.top().second;
    pq.pop();

    if (curDist > dist[curNode]) continue;

    for (const auto &edge : graph[curNode]) {
      int nxtNode = edge.first;
      int nxtDist = curDist + edge.second;

      if (nxtDist < dist[nxtNode]) {
        dist[nxtNode] = nxtDist;
        pq.push({-nxtDist, nxtNode});
      }
    }
  }
  return dist;
}

TEST(DijkstraTest, SimpleGraphShortestPath) {
  // 4 nodes: 0, 1, 2, 3
  vector<vector<pair<int, int>>> graph(4);
  graph[0].push_back({1, 1});
  graph[0].push_back({2, 4});
  graph[1].push_back({2, 2});
  graph[1].push_back({3, 6});
  graph[2].push_back({3, 1});

  vector<int> dist = run_dijkstra(graph, 0);

  EXPECT_EQ(dist[0], 0);
  EXPECT_EQ(dist[1], 1);
  EXPECT_EQ(dist[2], 3); // 0 -> 1 (1) -> 2 (2) = 3
  EXPECT_EQ(dist[3], 4); // 0 -> 1 -> 2 -> 3 (1+2+1=4)
}

TEST(DijkstraTest, DisconnectedGraph) {
  vector<vector<pair<int, int>>> graph(3);
  graph[0].push_back({1, 5});

  vector<int> dist = run_dijkstra(graph, 0);

  EXPECT_EQ(dist[0], 0);
  EXPECT_EQ(dist[1], 5);
  EXPECT_EQ(dist[2], INF);
}
