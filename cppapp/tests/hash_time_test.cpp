#include <gtest/gtest.h>
#include <my/u.hpp>
#include <regex>
#include <thread>
#include <chrono>

TEST(HashTimeUnitTest, StringHashConsistency) {
    const char* str1 = "hello_world";
    const char* str2 = "hello_world";
    const char* str3 = "different_string";

    size_t h1 = my::mstrhash(str1);
    size_t h2 = my::mstrhash(str2);
    size_t h3 = my::mstrhash(str3);

    EXPECT_EQ(h1, h2);
    EXPECT_NE(h1, h3);
}

TEST(HashTimeUnitTest, HashModuloBounds) {
    size_t mod = 100;
    size_t h = my::mstrhash("test_string_for_modulo", mod);
    EXPECT_LT(h, mod);
}

TEST(HashTimeUnitTest, CurrentTimeMillisMonotonicity) {
    uint64_t t1 = my::curTimeMillis();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    uint64_t t2 = my::curTimeMillis();

    EXPECT_GE(t2, t1);
}

TEST(HashTimeUnitTest, TimestampFormatting) {
    char buf[64]{'\0'};
    const char* res = my::tmstr(buf);
    ASSERT_NE(res, nullptr);

    // Expected format: YYYY-MM-DD HH:MM:SS.mmm
    std::string str(buf);
    EXPECT_EQ(str.length(), 23u);
    EXPECT_EQ(str[4], '-');
    EXPECT_EQ(str[7], '-');
    EXPECT_EQ(str[10], ' ');
    EXPECT_EQ(str[13], ':');
    EXPECT_EQ(str[16], ':');
    EXPECT_EQ(str[19], '.');
}
