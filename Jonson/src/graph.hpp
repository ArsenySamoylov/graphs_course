#pragma once

#include <algorithm>
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
    using list_it     = list_type::iterator;

    enum class EdgeOrientation {
        Oriented,
        NotOriented,
    };
    
private:
    using graph_type  = std::unordered_map<vertex_type, list_type>;

    graph_type g_;

public:
    weightedAdjListGraph() {}

    list_it find_edge (vertex_type src, vertex_type end) {
        auto cmp = [v = end] (edge_type e) { return e.first == v; };
        return std::find_if(g_[src].begin(), g_[src].end(), cmp);
    }

    bool changeWeight(vertex_type src, edge_type adj_v) {
        if(!g_.contains(src))
            return false;
        
        auto v = find_edge(src, adj_v.first);
        if(v == g_[src].end())
            return false;

        v->second = adj_v.second;
        return true;
    }

    void addEdge(vertex_type src, edge_type adj_v, EdgeOrientation orientation = EdgeOrientation::NotOriented) {
        g_[src].push_back(adj_v);
    
        if (orientation == EdgeOrientation::NotOriented) {
            g_[adj_v.first].push_back(edge_type{src, adj_v.second});;
        }
    }

    void addEdges(vertex_type src, std::vector<edge_type> adj_vs, EdgeOrientation orientation = EdgeOrientation::NotOriented) {
        for(auto edge : adj_vs)
            addEdge(src, edge, orientation);
    }

    void addVertex(vertex_type v) {
        assert(!g_.contains(v));
        g_[v];
    }

    void deleteVertex(vertex_type v) {
        g_.erase(v);
        // TODO: check that there are no edges to v
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

using distance_t         = ssize_t;
using vertex_t           = weightedAdjListGraph::vertex_type;
using dist_vect_t        = std::vector<distance_t>;

static constexpr distance_t InfDist = std::numeric_limits<distance_t>::max();

