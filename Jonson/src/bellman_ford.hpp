#pragma once

#include <optional>
#include <vector>

#include "graph.hpp"

inline std::optional<dist_vect_t> bellmanFord(vertex_t src, const weightedAdjListGraph& g) {
    if(!g.contains(src))
        return std::nullopt;
    
    const size_t V = g.V();
    std::vector<distance_t> dist(V, InfDist);
    dist[src] = 0;

    for(vertex_t _ = 0; _ < V - 1; ++_) {
        for(vertex_t u = 0; u < V; u++) {
            for(auto [v, w] : g.getAdjList(u)) {
                if(dist[u] != InfDist && dist[v] > dist[u] + w) {
                    dist[v] = dist[u] + w;
                }
            }
        }
    }

    for(vertex_t u = 0; u < V; u++) {
        for(auto [v, w] : g.getAdjList(u)) {
            if(dist[v] > dist[u] + w) {
                return std::nullopt;
            }
        }
    }

    return dist;
}