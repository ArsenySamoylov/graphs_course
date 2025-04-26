#pragma once

#include <vector>
#include <optional>

#include "dijkstra.hpp"
#include "bellman_ford.hpp"

using all_dist_vect_t = std::vector<dist_vect_t>;

inline std::optional<all_dist_vect_t> johnson(const weightedAdjListGraph& g) {
    if(g.empty())
        return std::nullopt;
        
    weightedAdjListGraph g1 = g;
    const vertex_t zeroWeightVertex = g.V();

    g1.addVertex(zeroWeightVertex);
    for(vertex_t i = 0; i < g.V(); ++i)
        g1.addEdge(zeroWeightVertex, {i, 0}, weightedAdjListGraph::EdgeOrientation::Oriented);
    
    const auto dist = bellmanFord(zeroWeightVertex, g1);
    if(!dist)
        return std::nullopt;
    
    for(vertex_t u = 0; u < g1.V() - 1; ++u)
        for(auto [v, w] : g1.getAdjList(u)) {
            auto new_w = w + (*dist)[u] - (*dist)[v];
            assert(new_w >= 0);
            g1.changeWeight(u, {v, new_w});
        } 
    
    g1.deleteVertex(zeroWeightVertex);

    all_dist_vect_t res(g1.V());

    for(vertex_t u = 0; u < g1.V(); ++u) {
        auto d = dijkstra(u, g1);
        assert(d);

        // Reweight back
        for(auto [v, w] : g1.getAdjList(u)) {
            auto new_w = (*d)[v] - (*dist)[u] + (*dist)[v];
            (*d)[v] = new_w;
        } 

        res[u] = std::move(*d);
    }

    return res;
}
