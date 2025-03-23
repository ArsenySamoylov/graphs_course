#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>
#include <string>
#include <format>
#include <cassert>

class NonCopyable {
  public:
    NonCopyable() {};
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

typedef int error_t;

template<typename T>
class rb_tree : NonCopyable {
    using size_t = std::size_t;

    struct node; // see "rb_tree_node.hpp" for definition

    node *root_ = nullptr;
    size_t size_ = 0;

    node* rotate(node*, int);
    void fix_insert(node*);
    node* bst_insert(T val);

    void graphvis_traverse(node*, std::string&);
    void get_preorder_impl(node*, std::vector<T>&);
  public:

    rb_tree() {}
    // rb_tree(std::initializer_list<T>);
    // ~rb_tree();

    void insert(T val);
    // error_t del(T val);

    // void merge(rb_tree&&);

    std::vector<T> get_preorder();
    // std::vector<T> get_postorder();

    void to_graphvis(std::string&);

};

#include "rb_tree_node.hpp"
#include "rb_tree_graphvis.hpp"

// Main logic
template<typename T>
rb_tree<T>::node* rb_tree<T>::rotate(node* n, const int dir) {
  assert(n);
  assert(dir == node::Left || dir == node::Right);

  auto *par = n->parent_;
  auto *suc = n->children_[node::reverse_dir(dir)];
  assert(suc);
  auto *suc_child = suc->children_[dir];

  n->children_[node::reverse_dir(dir)] = suc_child;
  if (suc_child) {
    suc_child->parent_ = n;
  }
  n->parent_ = suc;

  suc->children_[dir] = n;
  suc->parent_ = par;
  
  if (par) {
    par->children_[dir] = suc;
  } else {
    assert(n == root_);
    root_ = par;
  }

  return suc;
}

template<typename T>
void rb_tree<T>::fix_insert(node* n) {  
  (void)n;
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::bst_insert(const T val) {
  auto *n = new rb_tree<T>::node{};
  n->val_ = val;

  if(root_ == nullptr) {
    root_ = n;
    return root_;
    }

  node *parent = nullptr;
  node *current = root_;
  while(true) {
    parent = current;
    auto dir = val < current->val_ ? node::Left : node::Right;
    current = current->children_[dir];
    
    if(current == nullptr) {
      n->parent_ = parent;
      parent->children_[dir] = n;
      return n;       
      }
    }
  }

template<typename T>
void rb_tree<T>::insert(T val) {
  fix_insert(bst_insert(val));
}


template<typename T>
std::vector<T> rb_tree<T>::get_preorder() {
  std::vector<T> v;
  get_preorder_impl(root_, v);
  return v;
}

template<typename T>
void rb_tree<T>::get_preorder_impl(node* n, std::vector<T>& v) {
  if (n == nullptr) {
    return;
  }
  v.push_back(n->val_);
  get_preorder_impl(n->children_[node::Left], v);
  get_preorder_impl(n->children_[node::Right], v);
}