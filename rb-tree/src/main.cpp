#include <print>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "rb_tree.hpp"

void greeting() {
    std::println("Hello, this is Red-Black Tree CLI.");
}


namespace fs = std::filesystem;

class graphvisor{
  public:
    const std::string tmp = "tmp/";
    const std::string dot_path = tmp + "gv.dot";
    const std::string image_path = tmp + "gv.png";

    graphvisor() {
        fs::create_directory(tmp);
    }

    void draw(const std::string& buf) {
        std::ofstream dot{dot_path};
        dot << buf << std::endl;
        dot.close();
    }

    int render() {
        int code = std::system(std::format("dot -T png -o {} {}", image_path, dot_path).c_str());
        if(code) {
            return code;
        }
        
        code = std::system(std::format("open {}", image_path).c_str());
        return code;
    }

};

int main() {
    greeting();
    graphvisor gv;
    std::string dot_buf;

    rb_tree<int> tree;
    int val;
    while(std::cout<<"Input number: ", std::cin >> val) {
        tree.insert(val);
        
        tree.to_graphvis(dot_buf);
        gv.draw(dot_buf);
        dot_buf.clear();
        if (gv.render()) {
            std::cerr << "Can't render =(\n";
            return -1;
        }
        std::print("Preorder: ");
        for (auto v : tree.get_preorder()) {
            std::cout << v << ", ";
        }
        std::cout << std::endl;
    }

    return 0;
}
