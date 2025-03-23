#include <gtest/gtest.h>
#include <utility>
#include <vector>

#include "rb_tree.hpp"

using val_color = std::pair<int, bool>;
using tree_container = std::vector<val_color>;

TEST(Basic, empty) {
    rb_tree<int> t;
    EXPECT_EQ(t.size(), 0);
}

TEST(Insert, insert) {
    rb_tree<int> t;
    t.insert(42);

    tree_container res = {{42, false}};
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), res.size());
}

TEST(Basic, initializer_list) {
    rb_tree<int> t = {1, 2, 3, 4, 5};
    
    tree_container res = {{2, false}, {1, false}, {4, false}, {3, true}, {5, true}};
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), res.size());
}

TEST(Insert, random_order) {
    rb_tree<int> t;
    t.insert(10);
    t.insert(20);
    t.insert(30);
    t.insert(15);

    tree_container res = {{20, false}, {10, false}, {15, true}, {30, false}};
    
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), res.size());
}

TEST(Insert, increasing_order) {
    rb_tree<int> t;
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.insert(4);
    t.insert(5);

    tree_container res = {{2, false}, {1, false}, {4, false}, {3, true}, {5, true}};
    
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), res.size());
}

TEST(Insert, decreasing_order) {
    rb_tree<int> t;
    t.insert(5);
    t.insert(4);
    t.insert(3);
    t.insert(2);
    t.insert(1);

    tree_container res = {{4, false}, {2, false}, {1, true}, {3, true}, {5, false}};
    
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), res.size());
}

TEST(Insert, large_number) {
    rb_tree<int> t;
    for (int i = 1; i <= 100; ++i) {
        t.insert(i);
    }

    // Check if the size is correct
    EXPECT_EQ(t.size(), 100);
    EXPECT_FALSE(t.get_preorder().empty());
}

TEST(Insert, duplicates) {
    rb_tree<int> t;
    t.insert(10);
    t.insert(10);  // Insert duplicate value

    tree_container res = {{10, false}, {10, true}};
    
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), res.size());
}

TEST(Insert, with_rotations) {
    rb_tree<int> t;
    t.insert(10);
    t.insert(20);
    t.insert(30);  // This insertion should trigger rotations.

    tree_container res = {{20, false}, {10, true}, {30, true}};
    
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), res.size());
}
