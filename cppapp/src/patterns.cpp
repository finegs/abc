#include <my/patterns.hpp>
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>

namespace my {

// ------------------------------------------
// 1. SINGLETON PATTERN
// ------------------------------------------
SystemConfig& SystemConfig::getInstance() {
    static SystemConfig instance;
    return instance;
}

void SystemConfig::setProperty(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    properties_[key] = value;
}

std::string SystemConfig::getProperty(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    return defaultValue;
}

bool SystemConfig::hasProperty(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return properties_.find(key) != properties_.end();
}

void SystemConfig::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    properties_.clear();
}

// ------------------------------------------
// 2. FACTORY PATTERN
// ------------------------------------------
constexpr int INF = 99999999;

std::vector<int> DijkstraAlgorithm::computeShortestPaths(int numVertices, const std::vector<GraphEdge>& edges, int source) {
    std::vector<std::vector<std::pair<int, int>>> graph(numVertices);
    for (const auto& edge : edges) {
        if (edge.from >= 0 && edge.from < numVertices && edge.to >= 0 && edge.to < numVertices) {
            graph[edge.from].push_back({edge.to, edge.weight});
        }
    }

    std::vector<int> dist(numVertices, INF);
    std::priority_queue<std::pair<int, int>> pq;

    if (source >= 0 && source < numVertices) {
        dist[source] = 0;
        pq.push({0, source});
    }

    while (!pq.empty()) {
        int curDist = -pq.top().first;
        int curNode = pq.top().second;
        pq.pop();

        if (curDist > dist[curNode]) continue;

        for (const auto& neighbor : graph[curNode]) {
            int nxtNode = neighbor.first;
            int nxtDist = curDist + neighbor.second;

            if (nxtDist < dist[nxtNode]) {
                dist[nxtNode] = nxtDist;
                pq.push({-nxtDist, nxtNode});
            }
        }
    }

    return dist;
}

std::vector<int> BellmanFordAlgorithm::computeShortestPaths(int numVertices, const std::vector<GraphEdge>& edges, int source) {
    std::vector<int> dist(numVertices, INF);
    if (source >= 0 && source < numVertices) {
        dist[source] = 0;
    }

    for (int i = 0; i < numVertices - 1; ++i) {
        for (const auto& edge : edges) {
            if (dist[edge.from] != INF && dist[edge.from] + edge.weight < dist[edge.to]) {
                dist[edge.to] = dist[edge.from] + edge.weight;
            }
        }
    }

    return dist;
}

std::unique_ptr<IGraphAlgorithm> GraphAlgorithmFactory::createAlgorithm(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::Dijkstra:
            return std::make_unique<DijkstraAlgorithm>();
        case AlgorithmType::BellmanFord:
            return std::make_unique<BellmanFordAlgorithm>();
        default:
            return nullptr;
    }
}

// ------------------------------------------
// 3. OBSERVER PATTERN
// ------------------------------------------
void ParkingSubject::attach(std::shared_ptr<IParkingObserver> observer) {
    if (observer) {
        observers_.push_back(observer);
    }
}

void ParkingSubject::detach(std::shared_ptr<IParkingObserver> observer) {
    observers_.erase(
        std::remove_if(observers_.begin(), observers_.end(),
            [&observer](const std::weak_ptr<IParkingObserver>& wptr) {
                auto sptr = wptr.lock();
                return !sptr || sptr == observer;
            }),
        observers_.end()
    );
}

void ParkingSubject::notify(const ParkingEvent& event) {
    for (auto it = observers_.begin(); it != observers_.end(); ) {
        if (auto observer = it->lock()) {
            observer->onParkingEvent(event);
            ++it;
        } else {
            it = observers_.erase(it);
        }
    }
}

size_t ParkingSubject::observerCount() const {
    size_t count = 0;
    for (const auto& wptr : observers_) {
        if (!wptr.expired()) count++;
    }
    return count;
}

// ------------------------------------------
// 4. STRATEGY PATTERN
// ------------------------------------------
int StandardFeeStrategy::calculateFee(int durationMinutes, int baseRate) const {
    int hours = static_cast<int>(std::ceil(durationMinutes / 60.0));
    return hours * baseRate;
}

int DiscountFeeStrategy::calculateFee(int durationMinutes, int baseRate) const {
    int rawFee = StandardFeeStrategy().calculateFee(durationMinutes, baseRate);
    return static_cast<int>(rawFee * (1.0 - discountPercentage_));
}

int PeakHourFeeStrategy::calculateFee(int durationMinutes, int baseRate) const {
    int rawFee = StandardFeeStrategy().calculateFee(durationMinutes, baseRate);
    return static_cast<int>(rawFee * multiplier_);
}

// ------------------------------------------
// 5. ADAPTER PATTERN
// ------------------------------------------
std::vector<GraphEdge> LegacyListGraphAdapter::adaptLegacyListToGraphEdges(List* legacyList) {
    std::vector<GraphEdge> edges;
    if (!legacyList) return edges;

    for (Link curr = legacyList->begin(); curr != legacyList->end(); curr = curr->next) {
        if (curr->data) {
            LegacyEdgeData* legacyData = static_cast<LegacyEdgeData*>(curr->data);
            GraphEdge edge{legacyData->u, legacyData->v, legacyData->w};
            edges.push_back(edge);
        }
    }
    return edges;
}

} // namespace my
