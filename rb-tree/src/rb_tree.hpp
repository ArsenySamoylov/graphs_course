#pragma once
#include <cstddef>
#include <initializer_list>
#include <vector>
#include <string>
#include <format>

class NonCopyable {
  public:
    NonCopyable() {};
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

typedef int error_t;

template<typename T>
class rb_tree : NonCopyable {
  private:
    using size_t = std::size_t;

    struct node {
        T val_;
        node *left_, *right_;
        bool color_; // true = red, false = black

        static void to_graphvis(node* n, std::string& buf) {
        if(n) {
          buf += std::format("\t\tnode_{} [shape = Mrecord label = {}, fillcolor = {}, style=filled]\n", static_cast<void*>(n), n->val_, n->color_ ? "Red" : "Gray");
        }
        }
    };

    node *root_ = nullptr;
    size_t size_ = 0;

    // void left_rotate(node*);
    // void right_rotate(node*);
    // void insert_node(node*, node*);

    void graphvis_traverse(node*, std::string&);
  public:

    rb_tree() {}
    // rb_tree(std::initializer_list<T>);
    // ~rb_tree();

    void insert(T val);
    // error_t del(T val);

    // void merge(rb_tree&&);

    // std::vector<T> get_preorder();
    // std::vector<T> get_postorder();

    void to_graphvis(std::string&);

};

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

  node::to_graphvis(n, buf);

  if(n->left_) {
    node::to_graphvis(n->left_, buf);
    buf += std::format("\t\tnode_{} -> node_{}\n", static_cast<void*>(n), static_cast<void*>(n->left_));
    graphvis_traverse(n->left_, buf);
  }

  if(n->right_) {
    node::to_graphvis(n->right_, buf);
    buf += std::format("\t\tnode_{} -> node_{}\n", static_cast<void*>(n), static_cast<void*>(n->right_));
    graphvis_traverse(n->right_, buf);
  }
}

template<typename T>
void rb_tree<T>::insert(T val) {
  if(!root_) {
    root_ = new node{};
    root_->val_ = val;
    return;
  }

  node *n = root_;
  while(true) {
    if(!n->right_) {
      n->right_ = new node{};
      n->right_->val_ = val;
      return;
    }
    n = n->right_;
  }
}
