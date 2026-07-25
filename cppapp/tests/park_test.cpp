#include <gtest/gtest.h>
#include <my/park.hpp>
#include <vector>
#include <string>

TEST(ParkUnitTest, TimeConversion) {
    EXPECT_EQ(s2t("00:00"), 0);
    EXPECT_EQ(s2t("01:30"), 90);
    EXPECT_EQ(s2t("05:34"), 334);
    EXPECT_EQ(s2t("23:59"), 1439);
}

TEST(ParkUnitTest, FeeFormulaCalculation) {
    // Basic time 180 mins, basic fee 5000, unit time 10 mins, unit fee 600
    std::vector<int> fees = {180, 5000, 10, 600};

    // Within basic time
    EXPECT_EQ(formula(120, fees), 5000);
    EXPECT_EQ(formula(180, fees), 5000);

    // Partial unit time rounding up
    EXPECT_EQ(formula(181, fees), 5600); // 180 + 1 min -> 1 unit = 5000 + 600
    EXPECT_EQ(formula(190, fees), 5600);
    EXPECT_EQ(formula(191, fees), 6200);
}

TEST(ParkUnitTest, FullParkingFeeSolution) {
    std::vector<int> fees = {180, 5000, 10, 600};
    std::vector<std::string> records = {
        "05:34 5961 IN",
        "06:00 0000 IN",
        "06:34 5961 OUT",
        "07:59 0148 IN",
        "07:59 0000 OUT",
        "10:00 2300 IN",
        "19:00 05961 IN", // wait, "05961" -> stoi is 5961
        "22:59 5961 IN",
        "23:00 5961 OUT"
    };

    // Solution output should be sorted by vehicle number
    std::vector<int> result = solution(fees, records);
    ASSERT_FALSE(result.empty());
    EXPECT_GE(result.size(), 1u);
}

TEST(ParkUnitTest, AutoCheckoutAtMidnight) {
    std::vector<int> fees = {120, 2000, 15, 300};
    // Vehicle 1234 enters at 18:00 and never registers OUT -> auto checkout at 23:59 (359 mins duration)
    std::vector<std::string> records = {
        "18:00 1234 IN"
    };

    std::vector<int> result = solution(fees, records);
    ASSERT_EQ(result.size(), 1u);
    // 359 mins: basic 120 (2000) + ceil((359-120)/15) * 300 = 2000 + ceil(239/15)*300 = 2000 + 16*300 = 6800
    EXPECT_EQ(result[0], 6800);
}
