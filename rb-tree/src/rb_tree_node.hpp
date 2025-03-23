#pragma once

template<typename T>
struct rb_tree<T>::node{
    T val_;
    node *parent_;

    enum {
        Black = false,
        Red = true
    };
    bool color_; // true = red, false = black

    enum {
        Left = 0,
        Right = 1
    };
    node *children_[2];

    static_assert(1 - Right == Left);
    static_assert(1 - Left  == Right);
    static int reverse_dir(int dir) {
        assert(dir == Left || dir == Right);
        return 1 - dir;
    }
        
    void to_graphvis(std::string&);
};