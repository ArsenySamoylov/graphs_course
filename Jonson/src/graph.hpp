#pragma once

#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <cassert>


class weightedAdjListGraph {
public:
    using weight_type = int;
    using vertex_type = std::size_t;
    using edge_type   = std::pair<vertex_type, weight_type>;
    using list_type   = std::vector<edge_type>;
    
private:
    using graph_type  = std::unordered_map<vertex_type, list_type>;

    graph_type g_;

public:
    weightedAdjListGraph() {}

    void addEdge(vertex_type src, edge_type adj_v, bool oriented = false) {
        g_[src].push_back(adj_v);
    
        if (!oriented) {
            g_[adj_v.first].push_back(edge_type{src, adj_v.second});;
        }
    }

    void addEdges(vertex_type src, std::vector<edge_type> adj_vs, bool oriented = false) {
        for(auto edge : adj_vs)
            addEdge(src, edge, oriented);
    }

    void addVertex(vertex_type v) {
        assert(!g_.contains(v));
        g_[v];
    }
    
    void addVertices(std::vector<vertex_type> vv) {
        for(auto v : vv)
            addVertex(v);
    }

    const list_type& getAdjList (vertex_type v) const { 
        assert(g_.contains(v));
        return g_.at(v); 
    }

    size_t V() const { return g_.size(); }
    bool contains(vertex_type v) const { return g_.contains(v); }
};

using distance_t         = std::size_t;
using vertex_t           = weightedAdjListGraph::vertex_type;
using opt_dist_vect_type = std::optional<std::vector<distance_t>>; 

constexpr distance_t InfDist = std::numeric_limits<distance_t>::max();

