#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <memory>

using namespace std;

class TrieNode {
public:
    unordered_map<char, unique_ptr<TrieNode>> children;
    int value = 0;
    bool is_end = false;
};

class SimpleTrie {
private:
    unique_ptr<TrieNode> root;

public:
    SimpleTrie() : root(make_unique<TrieNode>()) {}

    void insert(const string &key, int val) {
        TrieNode *curr = root.get();
        for (char ch : key) {
            if (curr->children.find(ch) == curr->children.end()) {
                curr->children[ch] = make_unique<TrieNode>();
            }
            curr = curr->children[ch].get();
        }
        curr->value = val;
        curr->is_end = true;
    }

    int search(const string &key) const {
        const TrieNode *curr = root.get();
        for (char ch : key) {
            auto it = curr->children.find(ch);
            if (it == curr->children.end()) return -1;
            curr = it->second.get();
        }
        return curr->is_end ? curr->value : -1;
    }
};

TEST(TrieTest, InsertAndSearch) {
    SimpleTrie trie;
    trie.insert("apple", 100);
    trie.insert("app", 50);

    EXPECT_EQ(trie.search("apple"), 100);
    EXPECT_EQ(trie.search("app"), 50);
    EXPECT_EQ(trie.search("ap"), -1);
    EXPECT_EQ(trie.search("banana"), -1);
}
