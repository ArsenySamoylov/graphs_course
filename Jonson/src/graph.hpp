#pragma once

#include <cstddef>
#include <vector>
#include <utilities>


template<typename T>
class weightedAdjListGraph {
    using weight_type     = int;
    using value_type      = std::pair<T, weight_type>;
    using key_type        = std::size_t;
    using weightedAdjList = std::vector<std::vector<value_type>>;

    weightedAdjList g_;

    public:
    
    weightedAdjListGraph() {}


};