#pragma once

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>
#include <string>
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
  // fields
    using size_t = std::size_t;
    size_t size_ = 0;

    struct node; // see "rb_tree_node.hpp" for definition
    node *root_ = nullptr;

  // methods
    static node* rotate(rb_tree<T>*, node*, int);

    void fix_insert(node*);
    node* bst_insert(T val);

    node* search(T val);
    node* bst_prepare_to_delete(T val);
    void delete_fixup(node*);

    static std::pair<node*, node*> split(node*, node*);
    static node* unite(node*, node*);
    static node* join(node*, node*, node*);
    static node* join_right(node*, node*, node*);
    static node* join_left(node*, node*, node*);
    static size_t black_height(node*);

    static bool is_black(node* n) { return n == nullptr || n->color_ == node::Black; } 

    void free(node*);

    void get_preorder_impl(node*, std::vector<std::pair<T, bool>>&);
    void graphvis_traverse(node*, std::string&);

  public:

    rb_tree() {}
    rb_tree(std::initializer_list<T> l) {
      for(auto& v : l) {
        insert(std::move(v));
      }
    }

    ~rb_tree() { free(root_); }

    void insert(T val);
    // returns false if element wasn't found
    bool remove(T val);

    void merge(rb_tree&&);

    std::vector<std::pair<T, bool>> get_preorder();
    size_t size() const { return size_; }

    void to_graphvis(std::string&);

};

#include "rb_tree_node.hpp"
#include "rb_tree_graphvis.hpp"

// Main logic
template<typename T>
rb_tree<T>::node* rb_tree<T>::rotate(rb_tree<T>* tree, node* n, const int dir) {
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
    auto par_dir = par->children_[node::Left] == n ? node::Left : node::Right;
    par->children_[par_dir] = suc;
  } else {
    assert(tree);
    assert(n == tree->root_);
    assert(suc);
    tree->root_ = suc;
  }

  return suc;
}

template<typename T>
void rb_tree<T>::fix_insert(node* n) {  
  while(n != root_ && 
        n->color_ == node::Red && n->parent_->color_ == node::Red) {
      auto *parent = n->parent_;
      auto *grandparent = parent->parent_;
      
      const int parent_dir = parent == grandparent->children_[node::Left] ? node::Left : node::Right;
      node *uncle = grandparent->children_[node::reverse_dir(parent_dir)];
      if(uncle != nullptr && uncle->color_ == node::Red) {
        grandparent->color_ = node::Red;
        parent->color_      = node::Black;
        uncle->color_       = node::Black;
        n = grandparent;
      } else {
        const int dir = n == parent->children_[node::Left] ? node::Left : node::Right;
        if (dir == node::reverse_dir(parent_dir)) {
          rotate(this, parent, parent_dir);
          n = parent;
          parent = n->parent_;
        }
        n = parent;
        rotate(this, grandparent, node::reverse_dir(parent_dir));
        std::swap(parent->color_, grandparent->color_);
        return;
        }
      }

  root_->color_ = node::Black;  
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
  auto *n = bst_insert(val);
  n->color_ = node::Red;
  fix_insert(n);
  size_++;
}


template<typename T>
std::vector<std::pair<T, bool>> rb_tree<T>::get_preorder() {
  std::vector<std::pair<T, bool>> v;
  get_preorder_impl(root_, v);
  return v;
}

template<typename T>
void rb_tree<T>::get_preorder_impl(node* n, std::vector<std::pair<T, bool>>& v) {
  if (n == nullptr) {
    return;
  }
  v.push_back({n->val_, n->color_});
  get_preorder_impl(n->children_[node::Left], v);
  get_preorder_impl(n->children_[node::Right], v);
}


// Searches for the *deepest* node
template<typename T>
rb_tree<T>::node* rb_tree<T>::search(T val) {
  node *current = root_;
  node *result = nullptr;

  while (current != nullptr) {
    if (current->val_ == val) {
      result = current;
    }

    if (current->val_ > val) {
      current = current->children_[node::Left];
    } else {
      current = current->children_[node::Right];
    }
  }
  return result;
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::bst_prepare_to_delete(T val) {
  node *n = search(val);
  if (n == nullptr) {
    return nullptr;
  }

  int n_children = 0;
  n_children += (n->children_[node::Left] != nullptr);
  n_children += (n->children_[node::Right] != nullptr);  

  switch(n_children) {
    case 0:
      return n;
    case 1: {
      int dir = n->children_[node::Left] != nullptr ? node::Left : node::Right;
      node *child = n->children_[dir];
      std::swap(n->val_, child->val_);
      return child;
    } 
    case 2: {
      // Successor - leftmost element in right subtree
      node *successor = n->children_[node::Right];
      while (successor != nullptr && successor->children_[node::Left] != nullptr) {
        successor = successor->children_[node::Left];
      }
      std::swap(n->val_, successor->val_);
      return successor;
    }
    default:
      assert(0 && "Panic");
  }
}

template<typename T>
void rb_tree<T>::delete_fixup(node* n) {
  while(n != root_ && is_black(n)) {
    node *parent = n->parent_;
    int dir = n == parent->children_[node::Left] ? node::Left : node::Right;
    node *sibling = parent->children_[node::reverse_dir(dir)];

    if(is_black(sibling)) {
      parent->color_ = node::Red;
      rotate(this, parent, dir);
      sibling = parent->children_[node::reverse_dir(dir)];
    }

    // if sibling->children_ are Black
    if(sibling != nullptr && 
       is_black(sibling->children_[node::Left]) &&
       is_black(sibling->children_[node::Right])) {
      sibling->color_ = node::Red;
      n = parent;
      continue;
      }

    if(sibling != nullptr && 
       is_black(sibling->children_[node::reverse_dir(dir)])) {
      sibling->children_[dir]->color_ = node::Black;
      sibling->color_ = node::Red;
      rotate(this, sibling, node::reverse_dir(dir));
      sibling = parent->children_[node::reverse_dir(dir)];
      }
    
    if(sibling != nullptr) { 
      sibling->color_ = parent->color_;
      node *nephew = sibling->children_[node::reverse_dir(dir)];
      if(nephew != nullptr) {
        nephew->color_ = node::Black;
      }
    }
    parent->color_ = node::Black;
    rotate(this, parent, dir);
    n = root_;
  }

  n->color_ = node::Black;
}

template<typename T>
bool rb_tree<T>::remove(T val) {
  node* n = bst_prepare_to_delete(val);
  if (n == nullptr) {
    return false;
  }
  
  node *parent = n->parent_;
  node *successor = n->children_[node::Left] != nullptr ? n->children_[node::Left] : n->children_[node::Right];

  if (parent == nullptr) {
    root_ = successor;
  } else {
    int dir = n == parent->children_[node::Left] ? node::Left : node::Right;
    parent->children_[dir] = successor;
  }
   

  auto n_color = n->color_;
  delete(n);
  n = nullptr;

  if(successor != nullptr) {
    successor->parent_ = parent;

    if (n_color == node::Black) {
      delete_fixup(successor);
    }
  }

  size_--;
  return true;
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::unite(node* t1, node* t2) {
  if(t1 == nullptr) {
    return t2;
  }
  if(t2 == nullptr) {
    return t1;
  }

  auto [left, right] = split(t1, t2);
  node* new_left  = unite(left,  t2->children_[node::Left]);
  node* new_right = unite(right, t2->children_[node::Right]);
  return join(new_left, t2, new_right);
}

template<typename T>
std::pair<typename rb_tree<T>::node*, typename rb_tree<T>::node*> rb_tree<T>::split(node* n, node *separator) {
  if(n == nullptr) {
    return {nullptr, nullptr};
  }
  if(n->val_ == separator->val_) {
    return {n->children_[node::Left], n->children_[node::Right]};
  }
  if(separator->val_ < n->val_) {
    auto [left, right] = split(n->children_[node::Left], separator);
    return {left, join(right, n, n->children_[node::Right])};
  }
  auto [left, right] = split(n->children_[node::Right], separator);
  return {join(n->children_[node::Left], n, left), right};
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::join(node *l, node *separator, node *r) {
    if(black_height(l) > black_height(r)) {
      node *n = join_right(l, separator, r);
      if(n->color_ == node::Red && 
         (n->children_[node::Right] != nullptr && n->children_[node::Right]->color_ == node::Red)) {
         n->color_ = node::Black;
         }
      return n;
    }

    if(black_height(l) < black_height(r)) {
      node *n = join_left(l, separator, r);
      if(n->color_ == node::Red && 
         (n->children_[node::Left] != nullptr && n->children_[node::Left]->color_ == node::Red)) {
         n->color_ = node::Black;
         }
      return n;
    }

    separator->children_[node::Left] = l;
    separator->children_[node::Right] = r;
    if(l != nullptr) {
      l->parent_ = separator;
    }
    if(r != nullptr) {
      r->parent_ = separator;
    }
    
    if(is_black(l) && is_black(r)) {
      separator->color_ = node::Red;
    } else {
      separator->color_ = node::Black;
    }

    separator->parent_ = nullptr;
    return separator;
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::join_right(node *l, node *separator, node *r) {
  if(is_black(l) && black_height(l) == black_height(r)) {
    separator->color_ = node::Red;
    separator->children_[node::Left] = l;
    separator->children_[node::Right] = r;
    if(l != nullptr) {
      l->parent_ = separator;
    }
    if(r != nullptr) {
      r->parent_ = separator;
    }

    return separator;
  }

  node *right_join = join_right(l->children_[node::Right], separator, r);;
  right_join->parent_ = l;

  l->children_[node::Right] = right_join;
  l->parent_ = nullptr;

  if(is_black(l) && 
     l->children_[node::Right]->color_ == l->children_[node::Right]->children_[node::Right]->color_ &&
     l->children_[node::Right]->color_ == node::Red) {
    l->children_[node::Right]->children_[node::Right]->color_ = node::Black;
    return rotate(nullptr, l, node::Left);
    } 

  return l;
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::join_left(node *l, node *separator, node *r) {
  
  if(is_black(r) && black_height(l) == black_height(r)) {
    separator->color_ = node::Red;
    separator->children_[node::Left] = l;
    separator->children_[node::Right] = r;
    if(l != nullptr) {
      l->parent_ = separator;
    }
    if(r != nullptr) {
      r->parent_ = separator;
    }

    return separator;
  }


  node *left_join = join_left(l, separator, r->children_[node::Left]);;
  left_join->parent_ = r;

  r->children_[node::Left] = left_join;
  r->parent_ = nullptr;

  if(is_black(r) && 
    r->children_[node::Left]->color_ == r->children_[node::Left]->children_[node::Left]->color_ &&
    r->children_[node::Left]->color_ == node::Red) {
    r->children_[node::Left]->children_[node::Left]->color_ = node::Black;
    return rotate(nullptr, r, node::Right);
    } 

  return r;
}

template<typename T>
size_t rb_tree<T>::black_height(node* n) {
  size_t cnt = 0;
  while(n != nullptr) {
    if(is_black(n)) {
      cnt++;
    }
    n = n->children_[node::Left] != nullptr ? n->children_[node::Left] : n->children_[node::Right];
}
return cnt;
}

template<typename T>
void rb_tree<T>::merge(rb_tree<T> &&other) {
  root_ = unite(root_, other.root_);
  size_ += other.size_; // TODO this may be not valid because we allocating new nodes

  other.root_ = nullptr;
  other.size_ = 0;
}

// Other internals

template<typename T>
void rb_tree<T>::free(node* n) {
  if (n == nullptr) {
    return;
  }

  free(n->children_[node::Left]);
  free(n->children_[node::Right]);

  delete n;
}