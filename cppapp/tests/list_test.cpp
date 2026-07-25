#include <gtest/gtest.h>
#include <my/List.h>
#include <vector>

TEST(ListUnitTest, CreateAndDisposeEmptyList) {
    List* list = List::newList();
    ASSERT_NE(list, nullptr);
    EXPECT_EQ(list->usage, 0);
    EXPECT_EQ(list->begin(), list->end());
    list->dispose();
}

TEST(ListUnitTest, PushBackAndIterate) {
    List* list = List::newList();
    ASSERT_NE(list, nullptr);

    int val1 = 100, val2 = 200, val3 = 300;
    list->push_back(&val1);
    list->push_back(&val2);
    list->push_back(&val3);

    EXPECT_EQ(list->usage, 3);

    std::vector<int> results;
    for (Link curr = list->begin(); curr != list->end(); curr = curr->next) {
        results.push_back(*static_cast<int*>(curr->data));
    }

    std::vector<int> expected = {100, 200, 300};
    EXPECT_EQ(results, expected);

    list->dispose();
}

TEST(ListUnitTest, FindElements) {
    List* list = List::newList();
    int a = 10, b = 20, c = 30;
    list->push_back(&a);
    list->push_back(&b);

    EXPECT_TRUE(list->find(&a, nullptr));
    EXPECT_TRUE(list->find(&b, nullptr));
    EXPECT_FALSE(list->find(&c, nullptr));

    auto comp = [](Element data) -> int {
        int val = *static_cast<int*>(data);
        return (val == 20) ? 0 : 1;
    };

    EXPECT_TRUE(list->find(nullptr, comp));

    list->dispose();
}

TEST(ListUnitTest, EraseElements) {
    List* list = List::newList();
    int x = 1, y = 2, z = 3;
    list->push_back(&x);
    list->push_back(&y);
    list->push_back(&z);

    Link node_y = list->begin()->next;
    EXPECT_EQ(*static_cast<int*>(node_y->data), 2);

    list->erase(node_y);
    EXPECT_EQ(list->usage, 2);
    EXPECT_FALSE(list->find(&y, nullptr));

    std::vector<int> remaining;
    for (Link curr = list->begin(); curr != list->end(); curr = curr->next) {
        remaining.push_back(*static_cast<int*>(curr->data));
    }
    std::vector<int> expected = {1, 3};
    EXPECT_EQ(remaining, expected);

    list->dispose();
}
