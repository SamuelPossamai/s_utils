
#include <iostream>

#include "tree.h"

using namespace S_UTILS;

int main() {
    
    Tree<int> root(100);
    
    int k = 0;
    
    for(int i = 0; i < 5; i++){
        
        auto id = root.newChild(100*(i + 2));
        
        for(int j = 0; j < 5; j++) root.child(id).newChild(k++);
    }
    
    /*for(auto it = root.rbegin(); it != root.rend(); it++) {
        
        for(std::size_t i = 0; i < it->height(); i++) std::cout << " ";
        std::cout << it->data() << std::endl;
    }*/
    
    for(Tree<int>& tree : root) {
        
        for(std::size_t i = 0; i < tree.height(); i++) std::cout.put(' ');
        
        std::cout << tree.data();
        
        if(tree.isLeaf()) std::cout << " - leaf";
        if(tree.isRoot()) std::cout << " - root";
        
        std::cout << std::endl;
    }
}
