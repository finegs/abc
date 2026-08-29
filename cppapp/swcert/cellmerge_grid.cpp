#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <set>

using namespace std;

struct Cell {
    int id;
    double min_x, max_x;
    double min_y, max_y;
};

// 64비트 정수 키 생성을 위한 해시 함수 (Grid 좌표를 하나의 키로 묶음)
struct GridKey {
    int x, y;
    bool operator==(const GridKey& other) const {
        return x == other.x && y == other.y;
    }
};

struct GridKeyHash {
    size_t operator()(const GridKey& k) const {
        return (size_t)k.x * 73856093 ^ (size_t)k.y * 19349663;
    }
};

// Union-Find 연산
int findTissue(vector<int>& parent, int x) {
    if (parent[x] == x) return x;
    return parent[x] = findTissue(parent, parent[x]);
}

void unionCells(vector<int>& parent, int a, int b) {
    int rootA = findTissue(parent, a);
    int rootB = findTissue(parent, b);
    if (rootA != rootB) {
        if (rootA < rootB) parent[rootB] = rootA;
        else parent[rootA] = rootB;
    }
}

bool isOverwrapped(const Cell& c1, const Cell& c2, double limit_spec) {
    double overlap_x = min(c1.max_x, c2.max_x) - max(c1.min_x, c2.min_x);
    double overlap_y = min(c1.max_y, c2.max_y) - max(c1.min_y, c2.min_y);

    bool x_condition = (overlap_x > 0) && (overlap_x <= limit_spec) && (min(c1.max_y, c2.max_y) > max(c1.min_y, c2.min_y));
    bool y_condition = (overlap_y > 0) && (overlap_y <= limit_spec) && (min(c1.max_x, c2.max_x) > max(c1.min_x, c2.min_x));

    return x_condition || y_condition;
}

int main() {
    vector<Cell> cells = {
        {0, 0.0, 5.0, 0.0, 5.0},
        {1, 4.5, 9.5, 0.0, 5.0},
        {2, 12.0, 17.0, 0.0, 5.0},
        {3, 4.5, 9.5, 4.8, 9.8}
    };

    int n = cells.size();
    double boundary_limit = 0.6;
    
    // 일반적인 셀의 최대 크기 기준으로 그리드 크기 설정 (예: 5.0)
    double grid_size = 5.0; 

    vector<int> parent(n);
    for (int i = 0; i < n; i++) parent[i] = i;

    // 그리드 맵 정의 (그리드 좌표 -> 속한 셀 ID 목록)
    unordered_map<GridKey, vector<int>, GridKeyHash> grid;

    // 1. 셀들을 그리드에 등록 (셀이 여러 그리드에 걸칠 수 있으므로 범위를 모두 등록)
    for (const auto& cell : cells) {
        int min_gx = floor(cell.min_x / grid_size);
        int max_gx = floor(cell.max_x / grid_size);
        int min_gy = floor(cell.min_y / grid_size);
        int max_gy = floor(cell.max_y / grid_size);

        for (int gx = min_gx; gx <= max_gx; ++gx) {
            for (int gy = min_gy; gy <= max_gy; ++gy) {
                grid[{gx, gy}].push_back(cell.id);
            }
        }
    }

    // 2. 같은 그리드 칸에 존재하는 셀들끼리만 비교 연산 수행
    for (auto& [key, cell_ids] : grid) {
        int count = cell_ids.size();
        for (int i = 0; i < count; i++) {
            for (int j = i + 1; j < count; j++) {
                int id1 = cell_ids[i];
                int id2 = cell_ids[j];
                if (id1 != id2 && isOverwrapped(cells[id1], cells[id2], boundary_limit)) {
                    unionCells(parent, id1, id2);
                }
            }
        }
    }

    cout << "=== Grid Partitioning Results ===" << "\n";
    for (int i = 0; i < n; i++) {
        cout << "Cell ID " << i << " -> Tissue ID: " << findTissue(parent, i) << "\n";
    }
    return 0;
}

