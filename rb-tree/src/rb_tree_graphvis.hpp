#pragma once

#include "rb_tree.hpp"

template<typename T>
void rb_tree<T>::node::to_graphvis(std::string& buf) {
          buf += std::format("\t\tnode_{} [shape = Mrecord label = {}, fillcolor = {}, style=filled]\n", static_cast<void*>(this), val_, color_ ? "Red" : "Gray");
        }

template<typename T>
void rb_tree<T>::to_graphvis(std::string &buf) {
    buf += "digraph {\nrankdir = TB\n";
    graphvis_traverse(root_, buf);
    buf += "\n}\n";
}

template<typename T>
void rb_tree<T>::graphvis_traverse(node* n, std::string &buf) {
  if(n == nullptr) {
    return;
  }

  n->to_graphvis(buf);

  for (int i = 0; i < 2; ++i) {
    auto *child = n->children_[i];
    if (child) {
      child->to_graphvis(buf);
      buf += std::format("\t\tnode_{} -> node_{}\n", static_cast<void*>(n), static_cast<void*>(child));
      graphvis_traverse(child, buf);
    }
  }
}
