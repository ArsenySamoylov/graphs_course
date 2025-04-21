#include <gtest/gtest.h>

#include "bellman_ford.hpp"

TEST(BellmanFord, SimpleGraph) {
    // Create a simple graph
    weightedAdjListGraph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, {1, 4}); // Edge from 0 to 1 with weight 4
    g.addEdge(0, {2, 1}); // Edge from 0 to 2 with weight 1
    g.addEdge(2, {1, 2}); // Edge from 2 to 1 with weight 2

    auto result = bellmanFord(0, g);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()[1], 3); // Shortest path from 0 to 1 is through 2
    EXPECT_EQ(result.value()[2], 1); // Shortest path from 0 to 2 is direct
}

TEST(BellmanFord, GraphWithNegativeWeights) {
    // Create a graph with negative weights but no negative cycles
    weightedAdjListGraph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, {1, -2}, true); // Edge from 0 to 1 with weight -2
    g.addEdge(1, {2, -3}, true); // Edge from 1 to 2 with weight -3
    g.addEdge(0, {2,  4}, true); // Edge from 0 to 2 with weight 4

    auto result = bellmanFord(0, g);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()[1], -2); // Shortest path from 0 to 1 is direct
    EXPECT_EQ(result.value()[2], -5); // Shortest path from 0 to 2 is through 1
}

TEST(BellmanFord, GraphWithNegativeCycle) {
    // Create a graph with a negative cycle
    weightedAdjListGraph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, {1, -1});
    g.addEdge(1, {2, -1});
    g.addEdge(2, {0, -1}); // This creates a negative cycle

    auto result = bellmanFord(0, g);
    ASSERT_FALSE(result.has_value()); // Should return nullopt due to negative cycle
}

TEST(BellmanFord, NonExistentSourceVertex) {
    // Test with a non-existent source vertex
    weightedAdjListGraph g;
    auto result = bellmanFord(99, g); // Assuming vertex 99 does not exist
    ASSERT_FALSE(result.has_value()); // Should return nullopt since vertex does not exist
}

TEST(BellmanFord, EmptyGraph) {
    // Test with an empty graph
    weightedAdjListGraph g;
    auto result = bellmanFord(0, g); // Assuming vertex 0 does not exist in an empty graph
    ASSERT_FALSE(result.has_value()); // Should return nullopt since vertex does not exist
}