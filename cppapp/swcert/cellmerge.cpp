#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// 세포의 경계면 정보를 담는 구조체
struct Cell {
    int id;
    double min_x, max_x;
    double min_y, max_y;
};

// 1. Find 연산 (경로 압축 최적화)
int findTissue(vector<int>& parent, int x) {
    if (parent[x] == x) return x;
    return parent[x] = findTissue(parent, parent[x]);
}

// 2. Union 연산 (가장 작은 인덱스를 Chief Cell ID / Tissue ID로 설정)
void unionCells(vector<int>& parent, int a, int b) {
    int rootA = findTissue(parent, a);
    int rootB = findTissue(parent, b);
    
    if (rootA != rootB) {
        // 더 낮은 인덱스를 가진 세포를 Chief Cell로 설정
        if (rootA < rootB) {
            parent[rootB] = rootA;
        } else {
            parent[rootA] = rootB;
        }
    }
}

// 3. X축 또는 Y축 방향의 Overwrapped Boundary 조건 체크 함수
bool isOverwrapped(const Cell& c1, const Cell& c2, double limit_spec) {
    // X축 겹치는 구간 계산
    double overlap_x = min(c1.max_x, c2.max_x) - max(c1.min_x, c2.min_x);
    // Y축 겹치는 구간 계산
    double overlap_y = min(c1.max_y, c2.max_y) - max(c1.min_y, c2.min_y);

    // 두 셀이 물리적으로 겹쳐있는 상태에서, 경계면 크기가 기준치(limit_spec) 이내인지 확인
    bool x_condition = (overlap_x > 0) && (overlap_x <= limit_spec) && (min(c1.max_y, c2.max_y) > max(c1.min_y, c2.min_y));
    bool y_condition = (overlap_y > 0) && (overlap_y <= limit_spec) && (min(c1.max_x, c2.max_x) > max(c1.min_x, c2.min_x));

    return x_condition || y_condition;
}

int main() {
    // 샘플 데이터 구성 (0번 ~ 3번 셀)
    vector<Cell> cells = {
        {0, 0.0, 5.0, 0.0, 5.0},
        {1, 4.5, 9.5, 0.0, 5.0},  // 0번 셀과 X축으로 0.5만큼 겹침
        {2, 12.0, 17.0, 0.0, 5.0}, // 고립된 셀
        {3, 4.5, 9.5, 4.8, 9.8}   // 1번 셀과 Y축으로 0.2만큼 겹침
    };

    int n = cells.size();
    double boundary_limit = 0.6; // Overwrapped boundary limit spec

    // Union-Find 부모 테이블 초기화
    vector<int> parent(n);
    for (int i = 0; i < n; i++) parent[i] = i;

    // 모든 셀 쌍을 비교하며 조건 만족 시 병합 (O(N^2))
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (isOverwrapped(cells[i], cells[j], boundary_limit)) {
                unionCells(parent, cells[i].id, cells[j].id);
            }
        }
    }

    // 결과 출력 (각 Cell의 최종 Tissue ID 확인)
    cout << "=== Cell Merge Results ===" << "\n";
    for (int i = 0; i < n; i++) {
        int tissue_id = findTissue(parent, i);
        cout << "Cell ID " << i << " -> Tissue ID (Chief): " << tissue_id << "\n";
    }

    return 0;
}
