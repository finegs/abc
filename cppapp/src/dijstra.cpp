#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

#define INF 99999999

static int N = 10; // 노드의 개수가 10개라 가정.
static int E = 20; //

static char __VERSION[20] = "v.0.0.01_250824";

// vector<pair<int, int>> graph[N]; // 그래프 형태 선언

// 시작 위치와 정점의 개수, 인접 행렬을 받아
// 최소 거리 행렬을 반환함
vector<int> dijkstra(vector<pair<int, int>> *graph, int start = 0) {
  vector<int> dist(N, INF);          // 거리를 저장할 리스트 초기화
  priority_queue<pair<int, int>> pq; // 우선순위 큐 선언

  dist[start] = 0;     // 시작 노드 거리를 0으로 함
  pq.push({0, start}); // 우선순위 큐에 넣기

  cout << "[D]" << " len(graph) : " << N << std::endl;
  while (!pq.empty()) {
    int curDist = -pq.top().first; // 큐에서 꺼내 방문하고 있는 정점의 거리
    int curNode = pq.top().second; // 정점의 인덱스(번호)
    pq.pop();

    for (int i = 0; i < graph[curNode].size(); i++) {
      int nxtNode = graph[curNode][i].first;            // 인접 정점 번호
      int nxtDist = curDist + graph[curNode][i].second; // 인접 정점까지 거리

      if (nxtDist < dist[nxtNode]) // 지금 계산한 것이 기존 거리값보다 작다면
      {
        dist[nxtNode] = nxtDist;      // 거리값 업데이트
        pq.push({-nxtDist, nxtNode}); // 우선순위 큐에 넣기
      }
    }
  }
  return dist; // start 노드로부터 각 노드까지 최단거리를 담은 벡터 리턴
}

int main() {
  //
  cout << "Enter a N(Node Count) and E(Edge Count):";
  cout.flush();

  // 2025-08-24 for edge count with args
  cin >> N >> E;
  cout << "Node : " << N << ", Edge : " << E << std::endl;

  size_t node_len = static_cast<size_t>(N);
  vector<pair<int, int>> graph[node_len];

  cout << " Node_Len : " << node_len << std::endl;

  for (int i = 0; i < E; i++) {
    int from, to, cost; // 간선의 시작점, 끝점, 가중치
    cin >> from >> to >> cost;
    // if (graph[from])
    //   *graph[from] = new vector<pair<int, int>>();
    graph[from].push_back({to, cost}); // 무향 그래프라 가정하므로 시작점과
                                       // 끝점 둘 다 벡터에 넣어야 함
    // if (!graph[to])
    //   graph[to] = new vector<pair<int, int>>();
    graph[to].push_back({from, cost});
  }

  cout << "==========" << std::endl;
  for (int i = 0; i < N; i++) {
    vector<pair<int, int>> node_edges = graph[i];
    for (pair<int, int> ne : node_edges) {
      cout << i << " " << ne.first << " " << ne.second << std::endl;
    }
    i++;
  }

  cout << "==========" << std::endl;
  vector<int> dist = dijkstra(graph);
  cout << "끝점까지의 최단거리 : " << dist[N - 1] << endl;
  cout << " .Version : " << __VERSION << std::endl;

  // if (graph)
  //   delete[] graph;
  return 0;
}
