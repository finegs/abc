#include <gtest/gtest.h>
#include <my/patterns.hpp>
#include <my/List.h>
#include <thread>
#include <vector>
#include <string>

// ==========================================
// 1. SINGLETON PATTERN TEST
// ==========================================
TEST(DesignPatternTest, SingletonPatternInstanceAndProperties) {
    my::SystemConfig& config1 = my::SystemConfig::getInstance();
    my::SystemConfig& config2 = my::SystemConfig::getInstance();

    // Verify same instance reference
    EXPECT_EQ(&config1, &config2);

    config1.clear();
    config1.setProperty("app.name", "AntigravityApp");
    config1.setProperty("app.version", "2.0.0");

    EXPECT_EQ(config2.getProperty("app.name"), "AntigravityApp");
    EXPECT_EQ(config2.getProperty("app.version"), "2.0.0");
    EXPECT_EQ(config2.getProperty("non.existent", "default_val"), "default_val");
}

TEST(DesignPatternTest, SingletonPatternConcurrentAccess) {
    my::SystemConfig& config = my::SystemConfig::getInstance();
    config.clear();

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([i]() {
            my::SystemConfig::getInstance().setProperty("key_" + std::to_string(i), "val_" + std::to_string(i));
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(config.getProperty("key_" + std::to_string(i)), "val_" + std::to_string(i));
    }
}

// ==========================================
// 2. FACTORY PATTERN TEST
// ==========================================
TEST(DesignPatternTest, FactoryMethodCreatesCorrectAlgorithms) {
    auto dijkstra = my::GraphAlgorithmFactory::createAlgorithm(my::AlgorithmType::Dijkstra);
    ASSERT_NE(dijkstra, nullptr);
    EXPECT_EQ(dijkstra->getAlgorithmName(), "Dijkstra");

    auto bellmanFord = my::GraphAlgorithmFactory::createAlgorithm(my::AlgorithmType::BellmanFord);
    ASSERT_NE(bellmanFord, nullptr);
    EXPECT_EQ(bellmanFord->getAlgorithmName(), "Bellman-Ford");

    std::vector<my::GraphEdge> edges = {
        {0, 1, 10},
        {0, 2, 30},
        {1, 2, 10}
    };

    std::vector<int> distDijkstra = dijkstra->computeShortestPaths(3, edges, 0);
    std::vector<int> distBellman = bellmanFord->computeShortestPaths(3, edges, 0);

    std::vector<int> expected = {0, 10, 20};
    EXPECT_EQ(distDijkstra, expected);
    EXPECT_EQ(distBellman, expected);
}

// ==========================================
// 3. OBSERVER PATTERN TEST
// ==========================================
class ConcreteParkingObserver : public my::IParkingObserver {
public:
    void onParkingEvent(const my::ParkingEvent& event) override {
        eventsReceived.push_back(event);
    }
    std::vector<my::ParkingEvent> eventsReceived;
};

TEST(DesignPatternTest, ObserverPatternNotificationAndDetach) {
    my::ParkingSubject subject;

    auto observer1 = std::make_shared<ConcreteParkingObserver>();
    auto observer2 = std::make_shared<ConcreteParkingObserver>();

    subject.attach(observer1);
    subject.attach(observer2);
    EXPECT_EQ(subject.observerCount(), 2u);

    my::ParkingEvent event1{"CAR-101", "10:00", my::ParkingEventType::VehicleEntered};
    subject.notify(event1);

    EXPECT_EQ(observer1->eventsReceived.size(), 1u);
    EXPECT_EQ(observer2->eventsReceived.size(), 1u);
    EXPECT_EQ(observer1->eventsReceived[0].vehicleId, "CAR-101");

    subject.detach(observer1);
    EXPECT_EQ(subject.observerCount(), 1u);

    my::ParkingEvent event2{"CAR-101", "11:30", my::ParkingEventType::VehicleExited};
    subject.notify(event2);

    EXPECT_EQ(observer1->eventsReceived.size(), 1u); // No new event
    EXPECT_EQ(observer2->eventsReceived.size(), 2u); // Received event2
}

// ==========================================
// 4. STRATEGY PATTERN TEST
// ==========================================
TEST(DesignPatternTest, StrategyPatternFeeCalculation) {
    auto standard = std::make_shared<my::StandardFeeStrategy>();
    auto discount = std::make_shared<my::DiscountFeeStrategy>(0.20); // 20% discount
    auto peak = std::make_shared<my::PeakHourFeeStrategy>(1.5);     // 1.5x peak surge

    my::ParkingFeeContext context(standard);

    // 90 minutes -> rounded up to 2 hours @ 1000 base rate = 2000
    EXPECT_EQ(context.calculate(90, 1000), 2000);
    EXPECT_EQ(context.currentStrategyName(), "Standard");

    // Switch strategy to Discount
    context.setStrategy(discount);
    EXPECT_EQ(context.calculate(90, 1000), 1600); // 2000 * 0.80 = 1600
    EXPECT_EQ(context.currentStrategyName(), "Discount");

    // Switch strategy to Peak Hour
    context.setStrategy(peak);
    EXPECT_EQ(context.calculate(90, 1000), 3000); // 2000 * 1.5 = 3000
    EXPECT_EQ(context.currentStrategyName(), "PeakHour");
}

// ==========================================
// 5. ADAPTER PATTERN TEST
// ==========================================
TEST(DesignPatternTest, AdapterPatternLegacyListToGraphEdges) {
    List* legacyList = List::newList();
    ASSERT_NE(legacyList, nullptr);

    my::LegacyEdgeData e1{0, 1, 15};
    my::LegacyEdgeData e2{1, 2, 25};
    my::LegacyEdgeData e3{0, 2, 50};

    legacyList->push_back(&e1);
    legacyList->push_back(&e2);
    legacyList->push_back(&e3);

    // Adapt legacy double-linked list to std::vector<GraphEdge>
    std::vector<my::GraphEdge> modernEdges = my::LegacyListGraphAdapter::adaptLegacyListToGraphEdges(legacyList);

    ASSERT_EQ(modernEdges.size(), 3u);
    EXPECT_EQ(modernEdges[0].from, 0);
    EXPECT_EQ(modernEdges[0].to, 1);
    EXPECT_EQ(modernEdges[0].weight, 15);

    EXPECT_EQ(modernEdges[1].from, 1);
    EXPECT_EQ(modernEdges[1].to, 2);
    EXPECT_EQ(modernEdges[1].weight, 25);

    // Pass adapted edges directly to Factory-created algorithm (Integration of Adapter + Factory!)
    auto algorithm = my::GraphAlgorithmFactory::createAlgorithm(my::AlgorithmType::Dijkstra);
    std::vector<int> distances = algorithm->computeShortestPaths(3, modernEdges, 0);

    EXPECT_EQ(distances[2], 40); // 0->1 (15) + 1->2 (25) = 40

    legacyList->dispose();
}
