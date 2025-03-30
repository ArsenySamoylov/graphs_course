#include <gtest/gtest.h>
#include <utility>
#include <vector>

#include "rb_tree.hpp"

using val_color = std::pair<int, bool>;
using tree_container = std::vector<val_color>;
using namespace rb;

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

// Test for deleting from an empty tree
TEST(Delete, empty) {
    rb_tree<int> t;
    EXPECT_FALSE(t.remove(42));  // Attempting to remove from an empty tree
    EXPECT_EQ(t.size(), 0);
}

// Test for deleting a single element
TEST(Delete, single_element) {
    rb_tree<int> t;
    t.insert(42);
    EXPECT_TRUE(t.remove(42));
    EXPECT_EQ(t.size(), 0);
}

// Test for deleting a leaf node
TEST(Delete, delete_leaf) {
    rb_tree<int> t;
    t.insert(10);
    t.insert(20);
    t.insert(30);  // Structure: 20 (root), with 10 (left) and 30 (right)
    
    EXPECT_TRUE(t.remove(30));  // Deleting a leaf node

    tree_container res = {{20, false}, {10, true}};  // Expecting 20 as root and 10 as left child
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), 2);
}

// Test for deleting a node with one child
TEST(Delete, delete_node_with_one_child) {
    rb_tree<int> t;
    t.insert(10);
    t.insert(20);
    t.insert(15);  // Structure: 20 (root), with 10 (left) and 15 (right)

    EXPECT_TRUE(t.remove(20));  // Deleting a node with one child (15)

    tree_container res = {{15, false}, {10, true}};  // Expecting 15 as root and 10 as left child
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), 2);
}

// Test for deleting a node with two children
TEST(Delete, delete_node_with_two_children) {
    rb_tree<int> t;
    t.insert(30);
    t.insert(20);
    t.insert(40);
    t.insert(10);
    t.insert(25);  
    /* Structure: 
        30
       /  \
      20   40
     / \
    10  25
    */

    EXPECT_TRUE(t.remove(20));  // Deleting a node with two children

    tree_container res = {{30, false}, {25, false}, {10, true}, {40, false}}; // Expecting proper structure after deletion
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), 4);
}

// Test for deleting the root node
TEST(Delete, delete_root) {
    rb_tree<int> t;
    t.insert(10);
    t.insert(20);
    t.insert(30);  // Structure: 20 (root)

    EXPECT_TRUE(t.remove(20));  // Deleting the root

    tree_container res = {{30, false}, {10, true}};  // Expecting new root to be 30
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), 2);
}

// Test for multiple deletions
TEST(Delete, multiple_deletions) {
    rb_tree<int> t;
    for (int i = 1; i <= 5; ++i) {
        t.insert(i);  // Insert elements from 1 to 5
    }

    EXPECT_TRUE(t.remove(3));  // Remove a middle element
    EXPECT_TRUE(t.remove(1));  // Remove a leaf element
    EXPECT_TRUE(t.remove(5));  // Remove another leaf element

    tree_container res = {{2, false}, {4, false}}; // Expecting proper structure after deletions
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), 2);
}

// Test for deleting non-existent element
TEST(Delete, delete_non_existent) {
    rb_tree<int> t;
    t.insert(10);
    t.insert(20);

    size_t initial_size = t.size();
    
    EXPECT_FALSE(t.remove(30));  // Attempting to delete a non-existent element
    EXPECT_EQ(t.size(), initial_size); // The size should remain the same
}

// Test for maintaining red-black properties after deletions
TEST(Delete, maintain_properties) {
    rb_tree<int> t;
    
    for (int i = 1; i <= 7; ++i) {
        t.insert(i);  // Insert elements to create a balanced tree
    }
    
    EXPECT_TRUE(t.remove(4)); // Remove an element and check if properties are maintained
    
    tree_container res = {{2, false}, {1, false}, {5, true}, {3, false}, {6, false}, {7, true}};
    
    EXPECT_EQ(t.get_preorder(), res); // Check structure after deletion
}

TEST(Merge, merge_two_non_empty_trees) {
    rb_tree<int> t1;
    t1.insert(10);
    t1.insert(20);
    t1.insert(30);

    rb_tree<int> t2;
    t2.insert(15);
    t2.insert(25);
    t2.insert(5);

    t1.merge(std::move(t2));

    tree_container res = {{15, false}, {5, false}, {10, true}, {25, false}, {20, true}, {30, true}};
    EXPECT_EQ(t1.get_preorder(), res);
    EXPECT_EQ(t1.size(), 6);
}

TEST(Merge, merge_with_empty_tree) {
    rb_tree<int> t1;
    t1.insert(1);
    t1.insert(2);
    
    rb_tree<int> empty_tree;

    t1.merge(std::move(empty_tree));

    tree_container res = {{1, false}, {2, true}};
    EXPECT_EQ(t1.get_preorder(), res);
    EXPECT_EQ(t1.size(), 2);
}

TEST(Merge, merge_empty_tree_into_non_empty) {
    rb_tree<int> empty_tree;
    
    rb_tree<int> t1;
    t1.insert(10);
    t1.insert(20);

    t1.merge(std::move(empty_tree));

    tree_container res = {{10, false}, {20, true}};
    EXPECT_EQ(t1.get_preorder(), res);
    EXPECT_EQ(t1.size(), 2);
}

TEST(Merge, self_merge) {
    rb_tree<int> t;
    t.insert(10);
    t.insert(20);

    t.merge(std::move(t)); // Merging with itself

    tree_container res = {{10, false}, {20, true}};
    EXPECT_EQ(t.get_preorder(), res);
    EXPECT_EQ(t.size(), 2);
}

TEST(Merge, merge_with_overlapping_elements) {
    rb_tree<int> t1;
    t1.insert(10);
    t1.insert(20);
    
    rb_tree<int> t2;
    t2.insert(15);
    t2.insert(20); // This is a duplicate
    t2.insert(25);

    t1.merge(std::move(t2));

    tree_container res = {{20, false}, {15, false}, {10, true}, {25, false}};
    EXPECT_EQ(t1.get_preorder(), res);
    // TODO: FIXME
    // EXPECT_EQ(t1.size(), 4); // Size should be 4 (duplicates are not counted)
}

TEST(Merge, merge_large_trees) {
    rb_tree<int> t1;
    for (int i = 0; i < 50; ++i) {
        t1.insert(i * 2); // Even numbers
    }

    rb_tree<int> t2;
    for (int i = 0; i < 50; ++i) {
        t2.insert(i * 2 + 1); // Odd numbers
    }

    t1.merge(std::move(t2));

    // Check if the size is correct (should be 100)
    EXPECT_EQ(t1.size(), 100);

    // Check if all numbers from 0 to 99 are present
    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(t1.contains(i));
    }
}