#include <gtest/gtest.h>
#include <item.hpp>
#include <set>
#include <unordered_map>
#include <sstream>
#include <vector>

TEST(ItemUnitTest, DefaultAndParameterizedConstructor) {
    Item default_item;
    EXPECT_EQ(default_item.x, 0);
    EXPECT_EQ(default_item.y, 0);

    Item p_item(10, 20);
    EXPECT_EQ(p_item.x, 10);
    EXPECT_EQ(p_item.y, 20);
}

TEST(ItemUnitTest, CopyAndAssignment) {
    Item a(5, 15);
    Item b = a;
    EXPECT_EQ(b.x, 5);
    EXPECT_EQ(b.y, 15);

    Item c(1, 1);
    c = a;
    EXPECT_EQ(c.x, 5);
    EXPECT_EQ(c.y, 15);
}

TEST(ItemUnitTest, EqualityAndStreamOperators) {
    Item a(3, 4);
    Item b(3, 4);
    Item c(3, 5);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);

    std::stringstream ss;
    ss << a;
    EXPECT_EQ(ss.str(), "{\"x\":\"3\", \"y\":\"4\"}");
}

TEST(ItemUnitTest, ContainerStreamOperators) {
    std::vector<Item> v = {Item(1, 2), Item(3, 4)};
    std::stringstream ss;
    ss << v;
    EXPECT_EQ(ss.str(), "[{\"x\":\"1\", \"y\":\"2\"}, {\"x\":\"3\", \"y\":\"4\"}]");
}

TEST(ItemUnitTest, CustomSearchAlgorithm) {
    std::vector<Item> vec = {Item(1, 2), Item(3, 4), Item(5, 6)};
    
    auto it_found = mysearch(vec.begin(), vec.end(), Item(3, 4));
    ASSERT_NE(it_found, vec.end());
    EXPECT_EQ(it_found->x, 3);
    EXPECT_EQ(it_found->y, 4);

    auto it_not_found = mysearch(vec.begin(), vec.end(), Item(9, 9));
    EXPECT_EQ(it_not_found, vec.end());
}

TEST(ItemUnitTest, HashSetAndMapUsage) {
    std::unordered_map<Item, int> map;
    Item i1(10, 20);
    Item i2(30, 40);

    map[i1] = 100;
    map[i2] = 200;

    EXPECT_EQ(map[i1], 100);
    EXPECT_EQ(map[i2], 200);
}
