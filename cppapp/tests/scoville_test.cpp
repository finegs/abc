#include <gtest/gtest.h>
#include <vector>
#include <queue>

using namespace std;

int run_scoville_solution(vector<int> scoville, int K) {
    int answer = 0;
    priority_queue<int, vector<int>, greater<int>> pq(scoville.begin(), scoville.end());

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

TEST(ScovilleTest, BasicCases) {
    EXPECT_EQ(run_scoville_solution({1, 2, 3, 9, 10, 12}, 7), 2);
    EXPECT_EQ(run_scoville_solution({1, 2, 3, 9, 10, 12}, 4), 2);
}

TEST(ScovilleTest, ImpossibleCase) {
    EXPECT_EQ(run_scoville_solution({1, 2}, 100), -1);
}

TEST(ScovilleTest, AlreadySatisfied) {
    EXPECT_EQ(run_scoville_solution({10, 20, 30}, 5), 0);
}
