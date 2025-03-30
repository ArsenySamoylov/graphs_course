#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
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

namespace rb {

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
    node* bst_insert(T&& val);

    node* search(const T& val);
    node* bst_prepare_to_delete(const T& val);
    void delete_fixup(node*);

    static std::pair<node*, node*> split(node*, node*);
    static node* unite(node*, node*);
    static node* join(node*, node*, node*);
    static node* join_right(node*, node*, node*);
    static node* join_left(node*, node*, node*);
    static size_t black_height(node*);

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
    bool contains(T&);

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

  node *par = n->parent_;
  node *suc = n->child(node::reverse_dir(dir));
  assert(suc);
  node *suc_child = suc->child(dir);

  n->child(node::reverse_dir(dir)) = suc_child;
  if (suc_child) {
    suc_child->parent_ = n;
  }
  n->parent_ = suc;

  suc->child(dir) = n;
  suc->parent_ = par;
  
  if (par) {
    auto par_dir = par->left() == n ? node::Left : node::Right;
    par->child(par_dir) = suc;
  } else {
    if(tree != nullptr) {
      assert(n == tree->root_);
      assert(suc);
      tree->root_ = suc;
    }
  }

  return suc;
}

template<typename T>
void rb_tree<T>::fix_insert(node* n) {  
  while(n != root_ && 
        node::is_red(n) && node::is_red(n->parent_)) {
      auto *parent = n->parent_;
      auto *grandparent = parent->parent_;
      
      const int parent_dir = parent == grandparent->left() ? node::Left : node::Right;
      node *uncle = grandparent->child(node::reverse_dir(parent_dir));
      if(uncle != nullptr && node::is_red(uncle)) {
        grandparent->color_ = node::Red;
        parent->color_      = node::Black;
        uncle->color_       = node::Black;
        n = grandparent;
      } else {
        const int dir = n == parent->left() ? node::Left : node::Right;
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
rb_tree<T>::node* rb_tree<T>::bst_insert(T&& val) {
  auto *n = new rb_tree<T>::node{};
  n->val_ = std::move(val);

  if(root_ == nullptr) {
    root_ = n;
    return root_;
    }

  node *parent = nullptr;
  node *current = root_;
  while(true) {
    parent = current;
    auto dir = val < current->val_ ? node::Left : node::Right;
    current = current->child(dir);
    
    if(current == nullptr) {
      n->parent_ = parent;
      parent->child(dir) = n;
      return n;       
      }
    }
  }

template<typename T>
void rb_tree<T>::insert(T val) {
  auto *n = bst_insert(std::move(val));
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
  get_preorder_impl(n->left(), v);
  get_preorder_impl(n->right(), v);
}


// Searches for the *deepest* node
template<typename T>
rb_tree<T>::node* rb_tree<T>::search(const T& val) {
  node *current = root_;
  node *result = nullptr;

  while (current != nullptr) {
    if (current->val_ == val) {
      result = current;
    }

    if (current->val_ > val) {
      current = current->left();
    } else {
      current = current->right();
    }
  }
  return result;
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::bst_prepare_to_delete(const T& val) {
  node *n = search(val);
  if (n == nullptr) {
    return nullptr;
  }

  int n_children = 0;
  n_children += (n->left() != nullptr);
  n_children += (n->right() != nullptr);  

  switch(n_children) {
    case 0:
      return n;
    case 1: {
      int dir = n->left() != nullptr ? node::Left : node::Right;
      node *child = n->child(dir);
      std::swap(n->val_, child->val_);
      return child;
    } 
    case 2: {
      // Successor - leftmost element in right subtree
      node *successor = n->right();
      while (successor != nullptr && successor->left() != nullptr) {
        successor = successor->left();
      }
      std::swap(n->val_, successor->val_);
      return successor;
    }
    default:
      assert(0 && "Panic");
      throw std::runtime_error("Unexpected number of children in rb_tree");
  }
}

template<typename T>
void rb_tree<T>::delete_fixup(node* n) {
  while(n != root_ && node::is_black(n)) {
    node *parent = n->parent_;
    int dir = n == parent->left() ? node::Left : node::Right;
    node *sibling = parent->child(node::reverse_dir(dir));

    if(node::is_black(sibling)) {
      parent->color_ = node::Red;
      rotate(this, parent, dir);
      sibling = parent->child(node::reverse_dir(dir));
    }

    // if sibling->children_ are Black
    if(sibling != nullptr && 
       node::is_black(sibling->left()) &&
       node::is_black(sibling->right())) {
      sibling->color_ = node::Red;
      n = parent;
      continue;
      }

    if(sibling != nullptr && 
       node::is_black(sibling->child(node::reverse_dir(dir)))) {
      sibling->child(dir)->color_ = node::Black;
      sibling->color_ = node::Red;
      rotate(this, sibling, node::reverse_dir(dir));
      sibling = parent->child(node::reverse_dir(dir));
      }
    
    if(sibling != nullptr) { 
      sibling->color_ = parent->color_;
      node *nephew = sibling->child(node::reverse_dir(dir));
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
  node *successor = n->left() != nullptr ? n->left() : n->right();

  if (parent == nullptr) {
    root_ = successor;
  } else {
    int dir = n == parent->left() ? node::Left : node::Right;
    parent->child(dir) = successor;
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
  node* new_left  = unite(left,  t2->left());
  node* new_right = unite(right, t2->right());
  return join(new_left, t2, new_right);
}

template<typename T>
std::pair<typename rb_tree<T>::node*, typename rb_tree<T>::node*> rb_tree<T>::split(node* n, node *separator) {
  if(n == nullptr) {
    return {nullptr, nullptr};
  }
  if(n->val_ == separator->val_) {
    return {n->left(), n->right()};
  }
  if(separator->val_ < n->val_) {
    auto [left, right] = split(n->left(), separator);
    return {left, join(right, n, n->right())};
  }
  auto [left, right] = split(n->right(), separator);
  return {join(n->left(), n, left), right};
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::join(node *l, node *separator, node *r) {
    if(black_height(l) > black_height(r)) {
      node *n = join_right(l, separator, r);
      if(node::is_red(n) && node::is_red(n->right())) {
         n->color_ = node::Black;
         }
      return n;
    }

    if(black_height(l) < black_height(r)) {
      node *n = join_left(l, separator, r);
      if(node::is_red(n) && node::is_red(n->left())) {
         n->color_ = node::Black;
         }
      return n;
    }

    separator->left() = l;
    separator->right() = r;
    if(l != nullptr) {
      l->parent_ = separator;
    }
    if(r != nullptr) {
      r->parent_ = separator;
    }
    
    if(node::is_black(l) && node::is_black(r)) {
      separator->color_ = node::Red;
    } else {
      separator->color_ = node::Black;
    }

    separator->parent_ = nullptr;
    return separator;
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::join_right(node *l, node *separator, node *r) {
  assert(separator);

  if(node::is_black(l) && black_height(l) == black_height(r)) {
    separator->color_ = node::Red;
    separator->left() = l;
    separator->right() = r;
    if(l != nullptr) {
      l->parent_ = separator;
    }
    if(r != nullptr) {
      r->parent_ = separator;
    }

    return separator;
  }

  node *right_join = join_right(l->right(), separator, r);;
  right_join->parent_ = l;

  l->right() = right_join;
  l->parent_ = nullptr;

  if(node::is_black(l) && 
     node::is_red(l->right()) && node::is_red(l->right()->right())) {
    l->right()->right()->color_ = node::Black;
    return rotate(nullptr, l, node::Left);
    } 

  return l;
}

template<typename T>
rb_tree<T>::node* rb_tree<T>::join_left(node *l, node *separator, node *r) {
  
  if(node::is_black(r) && black_height(l) == black_height(r)) {
    separator->color_ = node::Red;
    separator->left() = l;
    separator->right() = r;
    if(l != nullptr) {
      l->parent_ = separator;
    }
    if(r != nullptr) {
      r->parent_ = separator;
    }

    return separator;
  }


  node *left_join = join_left(l, separator, r->left());;
  left_join->parent_ = r;

  r->left() = left_join;
  r->parent_ = nullptr;

  if(node::is_black(r) && 
    node::is_red(r->left()) && node::is_red(r->left()->left())) {
    r->left()->left()->color_ = node::Black;
    return rotate(nullptr, r, node::Right);
    } 

  return r;
}

template<typename T>
size_t rb_tree<T>::black_height(node* n) {
  size_t cnt = 0;
  while(n != nullptr) {
    if(node::is_black(n)) {
      cnt++;
    }
    n = n->left() != nullptr ? n->left() : n->right();
}
return cnt;
}

template<typename T>
void rb_tree<T>::merge(rb_tree<T> &&other) {
  if(&other == this) {
    return;
  }
  root_ = unite(root_, other.root_);
  if(root_ != nullptr) {
    root_->color_ = node::Black;
  }

  size_ += other.size_; // TODO this may be not valid because we allocating new nodes

  other.root_ = nullptr;
  other.size_ = 0;
}

template<typename T>
bool rb_tree<T>::contains(T& val) {
  return search(val) != nullptr;
}
// Other internals

template<typename T>
void rb_tree<T>::free(node* n) {
  if (n == nullptr) {
    return;
  }

  free(n->left());
  free(n->right());

  delete n;
}

} // namespace rb