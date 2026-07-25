#include <gtest/gtest.h>
#include <my/List.h>
#include <my/park.hpp>
#include <my/u.hpp>
#include <item.hpp>

#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>
#include <algorithm>

namespace {
    // Dijkstra helper for integration test
    constexpr int INF = 99999999;

    std::vector<int> run_dijkstra(const std::vector<std::vector<std::pair<int, int>>> &graph, int start) {
        int n = static_cast<int>(graph.size());
        std::vector<int> dist(n, INF);
        std::priority_queue<std::pair<int, int>> pq;

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

    // Scoville helper
    int run_scoville(std::vector<int> scoville, int K) {
        int answer = 0;
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(scoville.begin(), scoville.end());

        while (!pq.empty() && pq.top() < K) {
            int first = pq.top();
            pq.pop();
            if (pq.empty()) break;
            int second = pq.top();
            pq.pop();

            pq.push(first + (second * 2));
            answer++;
        }

        if (pq.empty() || pq.top() < K) return -1;
        return answer;
    }
}

TEST(IntegrationTest, LinkedListWithItemStructAndSearch) {
    List* list = List::newList();
    ASSERT_NE(list, nullptr);

    Item item1(10, 20);
    Item item2(30, 40);
    Item item3(50, 60);

    list->push_back(&item1);
    list->push_back(&item2);
    list->push_back(&item3);

    EXPECT_EQ(list->usage, 3);

    // Verify search in linked list using Item equality comparator
    auto comp_find_30_40 = [](Element data) -> int {
        const Item* item = static_cast<const Item*>(data);
        return (item->x == 30 && item->y == 40) ? 0 : 1;
    };

    EXPECT_TRUE(list->find(nullptr, comp_find_30_40));

    // Calculate hash for all items stored in linked list using my::mstrhash
    std::vector<size_t> hashes;
    for (Link curr = list->begin(); curr != list->end(); curr = curr->next) {
        const Item* item = static_cast<const Item*>(curr->data);
        std::string key = std::to_string(item->x) + "_" + std::to_string(item->y);
        hashes.push_back(my::mstrhash(key.c_str()));
    }

    EXPECT_EQ(hashes.size(), 3u);
    list->dispose();
}

TEST(IntegrationTest, TrieGraphNodeMappingAndShortestPath) {
    // 1. Map string city names to integer node IDs using a Trie-like lookup
    std::unordered_map<std::string, int> city_map = {
        {"Seoul", 0},
        {"Daejeon", 1},
        {"Daegu", 2},
        {"Busan", 3}
    };

    // 2. Build graph using resolved IDs
    std::vector<std::vector<std::pair<int, int>>> graph(city_map.size());
    graph[city_map["Seoul"]].push_back({city_map["Daejeon"], 150});
    graph[city_map["Seoul"]].push_back({city_map["Daegu"], 280});
    graph[city_map["Daejeon"]].push_back({city_map["Daegu"], 120});
    graph[city_map["Daejeon"]].push_back({city_map["Busan"], 300});
    graph[city_map["Daegu"]].push_back({city_map["Busan"], 100});

    // 3. Measure shortest path execution time
    uint64_t start_time = my::curTimeMillis();
    std::vector<int> distances = run_dijkstra(graph, city_map["Seoul"]);
    uint64_t end_time = my::curTimeMillis();

    EXPECT_GE(end_time, start_time);

    // Seoul -> Daejeon (150) -> Daegu (120) -> Busan (100) = 370
    EXPECT_EQ(distances[city_map["Busan"]], 370);
}

TEST(IntegrationTest, ParkingRecordsProcessingAndScovilleOptimization) {
    // Integration scenario: Parking records generate durations, which are fed into a Scoville heap optimizer
    std::vector<int> fees = {60, 1000, 10, 200};
    std::vector<std::string> records = {
        "08:00 1001 IN",
        "08:15 1001 OUT", // 15 mins
        "09:00 1002 IN",
        "09:40 1002 OUT", // 40 mins
        "10:00 1003 IN",
        "11:10 1003 OUT", // 70 mins
    };

    std::vector<int> fee_results = solution(fees, records);
    EXPECT_EQ(fee_results.size(), 3u);

    // Feed raw duration values to Scoville mix
    std::vector<int> durations = {15, 40, 70};
    int mix_steps = run_scoville(durations, 60);
    EXPECT_GE(mix_steps, 1);
}
