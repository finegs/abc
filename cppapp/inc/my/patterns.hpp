#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <my/List.h>

namespace my {

// ==========================================
// 1. SINGLETON PATTERN
// ==========================================
class SystemConfig {
public:
    static SystemConfig& getInstance();
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key, const std::string& defaultValue = "") const;
    bool hasProperty(const std::string& key) const;
    void clear();

private:
    SystemConfig() = default;
    ~SystemConfig() = default;
    SystemConfig(const SystemConfig&) = delete;
    SystemConfig& operator=(const SystemConfig&) = delete;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::string> properties_;
};

// ==========================================
// 2. FACTORY METHOD PATTERN
// ==========================================
struct GraphEdge {
    int from;
    int to;
    int weight;
};

class IGraphAlgorithm {
public:
    virtual ~IGraphAlgorithm() = default;
    virtual std::vector<int> computeShortestPaths(int numVertices, const std::vector<GraphEdge>& edges, int source) = 0;
    virtual std::string getAlgorithmName() const = 0;
};

class DijkstraAlgorithm : public IGraphAlgorithm {
public:
    std::vector<int> computeShortestPaths(int numVertices, const std::vector<GraphEdge>& edges, int source) override;
    std::string getAlgorithmName() const override { return "Dijkstra"; }
};

class BellmanFordAlgorithm : public IGraphAlgorithm {
public:
    std::vector<int> computeShortestPaths(int numVertices, const std::vector<GraphEdge>& edges, int source) override;
    std::string getAlgorithmName() const override { return "Bellman-Ford"; }
};

enum class AlgorithmType {
    Dijkstra,
    BellmanFord
};

class GraphAlgorithmFactory {
public:
    static std::unique_ptr<IGraphAlgorithm> createAlgorithm(AlgorithmType type);
};

// ==========================================
// 3. OBSERVER PATTERN
// ==========================================
enum class ParkingEventType {
    VehicleEntered,
    VehicleExited
};

struct ParkingEvent {
    std::string vehicleId;
    std::string timestamp;
    ParkingEventType type;
};

class IParkingObserver {
public:
    virtual ~IParkingObserver() = default;
    virtual void onParkingEvent(const ParkingEvent& event) = 0;
};

class ParkingSubject {
public:
    void attach(std::shared_ptr<IParkingObserver> observer);
    void detach(std::shared_ptr<IParkingObserver> observer);
    void notify(const ParkingEvent& event);
    size_t observerCount() const;

private:
    std::vector<std::weak_ptr<IParkingObserver>> observers_;
};

// ==========================================
// 4. STRATEGY PATTERN
// ==========================================
class IFeeStrategy {
public:
    virtual ~IFeeStrategy() = default;
    virtual int calculateFee(int durationMinutes, int baseRate) const = 0;
    virtual std::string getStrategyName() const = 0;
};

class StandardFeeStrategy : public IFeeStrategy {
public:
    int calculateFee(int durationMinutes, int baseRate) const override;
    std::string getStrategyName() const override { return "Standard"; }
};

class DiscountFeeStrategy : public IFeeStrategy {
public:
    explicit DiscountFeeStrategy(double discountPercentage = 0.20) : discountPercentage_(discountPercentage) {}
    int calculateFee(int durationMinutes, int baseRate) const override;
    std::string getStrategyName() const override { return "Discount"; }

private:
    double discountPercentage_;
};

class PeakHourFeeStrategy : public IFeeStrategy {
public:
    explicit PeakHourFeeStrategy(double multiplier = 1.5) : multiplier_(multiplier) {}
    int calculateFee(int durationMinutes, int baseRate) const override;
    std::string getStrategyName() const override { return "PeakHour"; }

private:
    double multiplier_;
};

class ParkingFeeContext {
public:
    explicit ParkingFeeContext(std::shared_ptr<IFeeStrategy> strategy) : strategy_(std::move(strategy)) {}
    void setStrategy(std::shared_ptr<IFeeStrategy> strategy) { strategy_ = std::move(strategy); }
    int calculate(int durationMinutes, int baseRate) const {
        return strategy_ ? strategy_->calculateFee(durationMinutes, baseRate) : 0;
    }
    std::string currentStrategyName() const { return strategy_ ? strategy_->getStrategyName() : "None"; }

private:
    std::shared_ptr<IFeeStrategy> strategy_;
};

// ==========================================
// 5. ADAPTER PATTERN
// ==========================================
struct LegacyEdgeData {
    int u;
    int v;
    int w;
};

class LegacyListGraphAdapter {
public:
    static std::vector<GraphEdge> adaptLegacyListToGraphEdges(List* legacyList);
};

} // namespace my
