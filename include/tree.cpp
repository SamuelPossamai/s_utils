
#ifndef S_UTILS_TREE_CPP
#define S_UTILS_TREE_CPP

#include "tree.h"

namespace S_UTILS {

template<typename T, template <typename, typename...> typename Container, typename... ContainerArgs>
typename Tree<T, Container, ContainerArgs...>::size_type Tree<T, Container, ContainerArgs...>::newChild(const T& val) { 
        
    size_type id = _children_list.size();
    
    allocator_type alloc;
    
    _children_list.push_back(newTree(alloc, val));
    
    _children_list.back()->_parent = this;
    _children_list.back()->_child_id = id;
    _children_list.back()->_height = _height + 1;
    
    return id;
}

template<typename T, template <typename, typename...> typename Container, typename... ContainerArgs>
void Tree<T, Container, ContainerArgs...>::deleteChild(size_type id) { 
        
    allocator_type alloc;
    
    deleteTree(alloc, _children_list[id]);
    
    _children_list.back()->_child_id = id;
    
    _children_list[id] = _children_list.back();
    
    _children_list.pop_back();
}
    
template<typename T, template <typename, typename...> typename Container, typename... ContainerArgs>
template<typename U>
typename Tree<T, Container, ContainerArgs...>::template iterator_base<U>& Tree<T, Container, ContainerArgs...>::iterator_base<U>::operator++() {
            
    if(_t->isLeaf()) {
        
        while(1) {
            
            auto id = _t->childId();
            
            U *p = _t->parentPtr();
            
            if(p == nullptr || p == _base_p->parentPtr()) {
                
                _t = nullptr;
                
                break;
            }
            
            if(id + 1 < p->children()) {
                
                _t = p->childPtr(id + 1);
                
                break;
            }
            
            _t = p;
        }
        
    }else _t = _t->childPtr(0);
    
    return *this;
}

template<typename T, template <typename, typename...> typename Container, typename... ContainerArgs>
template<typename U>
typename Tree<T, Container, ContainerArgs...>::template iterator_base<U>& Tree<T, Container, ContainerArgs...>::iterator_base<U>::operator--() {
    
    if(_t == nullptr) {
        
        _t = _base_p;
        
        while(!_t->isLeaf()) _t = _t->childPtr(_t->children() - 1);
    }
    else if(_t->isLeaf()) {
    
        auto id = _t->childId();
        U *p = _t->parentPtr();
        
        if(p == nullptr) _t = nullptr;
        else if(id != 0) {
            
            _t = p->childPtr(id - 1);
            
            while(!_t->isLeaf()) _t = _t->childPtr(_t->children() - 1);
        }
        else _t = p;
    }
    else {
        
        while(_t->childId() == 0 && _t->parentPtr() != nullptr) _t = _t->parentPtr();
        
        U *p = _t->parentPtr();
        
        if(p != nullptr) {
            
            _t = p->childPtr(_t->childId() - 1);
            
            do {
                _t = _t->childPtr(_t->children() - 1);
            } 
            while(!_t->isLeaf());
        }
    }
    
    return *this;
}


};


#endif
