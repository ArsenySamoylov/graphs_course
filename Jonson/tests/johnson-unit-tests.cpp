#include <gtest/gtest.h>
#include "johnson.hpp" // Include your Johnson algorithm header

const auto Oriented = weightedAdjListGraph::EdgeOrientation::Oriented;

TEST(Johnson, SimpleGraph) {
    // Create a simple graph with undirected edges
    weightedAdjListGraph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, {1, 4}); // Undirected edge from 0 to 1 with weight 4
    g.addEdge(0, {2, 1}); // Undirected edge from 0 to 2 with weight 1
    g.addEdge(2, {1, 2}); // Undirected edge from 2 to 1 with weight 2

    auto result = johnson(g);
    ASSERT_TRUE(result.has_value());

    // Check distances
    EXPECT_EQ(result.value()[0][1], 3); // Distance from 0 to 1
    EXPECT_EQ(result.value()[0][2], 1); // Distance from 0 to 2
    EXPECT_EQ(result.value()[1][2], 2); // Distance from 1 to 2
}

TEST(Johnson, OrientedGraph) {
    // Create a graph with oriented edges
    weightedAdjListGraph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, {1, 4}, Oriented); // Edge from 0 to 1 with weight 4
    g.addEdge(0, {2, 1}, Oriented); // Edge from 0 to 2 with weight 1
    g.addEdge(2, {1, 2}, Oriented); // Edge from 2 to 1 with weight 2

    auto result = johnson(g);
    ASSERT_TRUE(result.has_value());

    // Check distances
    EXPECT_EQ(result.value()[0][1], 3); // Distance from 0 to 1
    EXPECT_EQ(result.value()[0][2], 1); // Distance from 0 to 2
    EXPECT_EQ(result.value()[1][2], InfDist); // Distance from 1 to 2 should be infinity
}

TEST(Johnson, GraphWithNegativeWeights) {
    // Create a graph with negative weights but no negative cycles
    weightedAdjListGraph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, {1, -2}, weightedAdjListGraph::EdgeOrientation::Oriented); // Edge from 0 to 1 with weight -2
    g.addEdge(1, {2, -3}, weightedAdjListGraph::EdgeOrientation::Oriented); // Edge from 1 to 2 with weight -3
    g.addEdge(0, {2,  4}, weightedAdjListGraph::EdgeOrientation::Oriented);  // Edge from 0 to 2 with weight 4

    auto result = johnson(g);
    ASSERT_TRUE(result.has_value());

    // Check distances
    EXPECT_EQ(result.value()[0][1], -2); // Distance from 0 to 1
    EXPECT_EQ(result.value()[0][2], -5); // Distance from 0 to 2
}

TEST(Johnson, GraphWithNegativeCycle) {
    // Create a graph with a negative cycle
    weightedAdjListGraph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, {1, -1}, Oriented);
    g.addEdge(1, {2, -1}, Oriented);
    g.addEdge(2, {0, -1}, Oriented); // This creates a negative cycle

    auto result = johnson(g);
    ASSERT_FALSE(result.has_value()); // Should return nullopt due to negative cycle
}

TEST(Johnson, NonExistentSourceVertex) {
    // Test with a non-existent source vertex (not applicable for all-pairs shortest path)
    weightedAdjListGraph g;
    g.addVertex(0);
    auto result = johnson(g); 
    ASSERT_TRUE(result.has_value()); // Should return a distance matrix for existing vertices
}

TEST(Johnson, EmptyGraph) {
    // Test with an empty graph
    weightedAdjListGraph g;
    auto result = johnson(g); 
    ASSERT_FALSE(result.has_value()); // Should return nullopt since the graph is empty
}
