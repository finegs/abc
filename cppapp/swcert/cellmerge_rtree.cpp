#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

using namespace std;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// Boost Geometry용 타입 정의
typedef bg::model::point<double, 2, bg::cs::cartesian> Point;
typedef bg::model::box<Point> Box;
typedef pair<Box, int> Value; // (Bounding Box, Cell ID)

struct Cell {
    int id;
    double min_x, max_x;
    double min_y, max_y;
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

    vector<int> parent(n);
    for (int i = 0; i < n; i++) parent[i] = i;

    // R-Tree 생성 및 데이터 입력 준비
    bgi::rtree<Value, bgi::quadratic<16>> rtree;

    for (const auto& cell : cells) {
        Box b(Point(cell.min_x, cell.min_y), Point(cell.max_x, cell.max_y));
        rtree.insert(make_pair(b, cell.id));
    }

    // 각 셀마다 R-Tree를 통해 충돌 가능성이 있는 이웃 셀들만 쿼리 후 병합
    for (const auto& cell : cells) {
        Box search_box(Point(cell.min_x, cell.min_y), Point(cell.max_x, cell.max_y));
        vector<Value> returned_results;
        
        // 현재 셀과 영역이 겹치거나 접하는 모든 사각형 검색
        rtree.query(bgi::intersects(search_box), back_inserter(returned_results));

        for (const auto& v : returned_results) {
            int neighbor_id = v.second;
            // ID가 더 큰 경우만 계산하여 중복 연산 방지
            if (cell.id < neighbor_id) {
                if (isOverwrapped(cell, cells[neighbor_id], boundary_limit)) {
                    unionCells(parent, cell.id, neighbor_id);
                }
            }
        }
    }

    cout << "=== Boost R-Tree Results ===" << "\n";
    for (int i = 0; i < n; i++) {
        cout << "Cell ID " << i << " -> Tissue ID: " << findTissue(parent, i) << "\n";
    }
    return 0;
}

