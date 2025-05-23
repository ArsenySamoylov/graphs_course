#pragma once

#include <optional>
#include <queue>
#include <vector>

#include "graph.hpp"

inline std::optional<dist_vect_t> dijkstra(vertex_t src, const weightedAdjListGraph& g) {
    if(!g.contains(src))
        return std::nullopt;

    using dist_and_vert_t = std::pair<distance_t, vertex_t>;

    std::priority_queue<dist_and_vert_t, std::vector<dist_and_vert_t>, std::greater<dist_and_vert_t>> q;
    std::vector<distance_t> dist(g.V(), InfDist);

    q.push({0, src});
    dist[src] = 0;

    while(!q.empty()) {
        vertex_t u = q.top().second;
        q.pop();
        
        for(auto [v, w] : g.getAdjList(u)) {
            if(w < 0)
                return std::nullopt;

            if(dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                q.push({dist[v], v});
            }
        }
    }

    return dist;
}