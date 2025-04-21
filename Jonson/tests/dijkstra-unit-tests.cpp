#include <gtest/gtest.h>

#include "dijkstra.hpp"

TEST(Dijkstra, basic) {
    weightedAdjListGraph graph;
    graph.addVertex(0);
    graph.addEdge(1, {0, 5});
    graph.addEdge(2, {1, 10});
    graph.addEdge(3, {2, 1});
    
    std::vector<distance_t> distances = *dijkstra(0, graph);
    
    EXPECT_EQ(distances[0], 0);  // Distance to itself
    EXPECT_EQ(distances[1], 5);  // Distance to vertex 1
    EXPECT_EQ(distances[2], 15); // Distance to vertex 2 (5 + 10)
    EXPECT_EQ(distances[3], 16); // Distance to vertex 3 (5 + 10 + 1)
}


TEST(Dijkstra, MultipleEdges) {
    weightedAdjListGraph graph;
    graph.addVertices({0, 1, 2, 3});
    graph.addEdge(0, {1, 1});
    graph.addEdge(0, {2, 4});
    graph.addEdge(1, {2, 2});
    graph.addEdge(1, {3, 6});
    graph.addEdge(2, {3, 1});
    
    std::vector<distance_t> distances = *dijkstra(0, graph);
    
    EXPECT_EQ(distances[0], 0); // Distance to itself
    EXPECT_EQ(distances[1], 1); // Shortest distance to vertex 1
    EXPECT_EQ(distances[2], 3); // Shortest distance to vertex 2 (1 + 2)
    EXPECT_EQ(distances[3], 4); // Shortest distance to vertex 3 (2 + 1)
}

TEST(Dijkstra, DisconnectedGraph) {
    weightedAdjListGraph graph;
    graph.addVertices({0, 1, 2, 3});
    graph.addEdge(0, {1, 5});
    
    std::vector<distance_t> distances = *dijkstra(0, graph);
    
    EXPECT_EQ(distances[0], 0); // Distance to itself
    EXPECT_EQ(distances[1], 5); // Distance to vertex 1
    EXPECT_EQ(distances[2], InfDist); // Vertex 2 is unreachable
    EXPECT_EQ(distances[3], InfDist); // Vertex 3 is unreachable
}

TEST(Dijkstra, EmptyGraph) {
    weightedAdjListGraph emptyGraph;
    
    // Since there are no vertices, we cannot run Dijkstra's algorithm.
    EXPECT_FALSE(dijkstra(0, emptyGraph));
}

TEST(Dijkstra, LargeGraph) {
    weightedAdjListGraph graph;
    
    for (size_t i = 0; i < 100; ++i) {
        graph.addVertex(i);
        if (i > 0) {
            graph.addEdge(i - 1, {i, i});
        }
    }
    
    std::vector<distance_t> distances = *dijkstra(0, graph);
    for (size_t i = 0; i < distances.size(); ++i) {
        EXPECT_EQ(distances[i], i * (i + 1) / 2); // Sum of first i natural numbers
    }
}