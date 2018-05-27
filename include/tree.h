
#ifndef S_UTILS_TREE_H
#define S_UTILS_TREE_H

#include <vector>

namespace S_UTILS {

template<typename T, template <typename, typename...> typename Container = std::vector, typename... ContainerArgs>
class Tree {
    
    template<typename U> class iterator_base;
    
    using ChildrenListType = Container<Tree *, ContainerArgs...>;

public:
    
    using size_type = typename ChildrenListType::size_type;
    using allocator_type = typename ChildrenListType::allocator_type::template rebind<Tree>::other;
    
    using iterator = iterator_base<Tree>;
    using const_iterator = iterator_base<const Tree>;
    
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    explicit Tree(const T& data = T()) : Tree(nullptr, 0, data) {}
    Tree(const Tree& t) = delete;
    
    //Tree(Tree&& t) : _parent(nullptr), _child_id(0), _height(0), _children_list(std::move(t._children_list)), _data(std::move(_data)) {}
    
    virtual ~Tree() { allocator_type alloc; for(Tree *t : _children_list) deleteTree(alloc, t); }

    Tree& operator=(const Tree&) = delete;
    
    iterator begin() { return iterator(this, this); }
    iterator end() { return iterator(nullptr, this); }
    
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    
    const_iterator cbegin() const { return const_iterator(this, this); }
    const_iterator cend() const { return const_iterator(nullptr, this); }
    
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    
    const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
    const_reverse_iterator rend() const { return reverse_iterator(begin()); }
    
    const_reverse_iterator crbegin() const { return reverse_iterator(end()); }
    const_reverse_iterator crend() const { return reverse_iterator(begin()); }
    
    size_type newChild(const T& val = T());
    
    void deleteChild(size_type id);
    
    Tree& child(size_t id) { return *_children_list[id]; }
    const Tree& child(size_t id) const { return *_children_list[id]; }
    
    Tree *childPtr(size_t id) { return _children_list[id]; }
    const Tree *childPtr(size_t id) const { return _children_list[id]; }
    
    size_type childId() const { return _child_id; }
    
    size_type children() const { return _children_list.size(); }
    bool isLeaf() const { return children() == 0; }
    bool isRoot() const { return _parent == nullptr; }
    
    Tree& parent() { return *_parent; }
    const Tree& parent() const { return *_parent; }
    
    Tree *parentPtr() { return _parent; }
    const Tree *parentPtr() const { return _parent; }
    
    T& data() { return _data; }
    const T& data() const { return _data; }
    
    size_type height() const { return _height; }
    
private:
    
    Tree(Tree *parent, size_type id, const T& data = T()) : _parent(parent), _child_id(id), _height(0), _data(data) {}
    
    template<typename... Args>
    Tree *newTree(allocator_type &alloc, Args&&... args) {

        Tree *ptr = alloc.allocate(1);
        
        std::allocator_traits<allocator_type>::construct(alloc, ptr, args...);
        
        return ptr;
    }
    
    void deleteTree(allocator_type &alloc, Tree *t) {
        
        std::allocator_traits<allocator_type>::destroy(alloc, t);
        
        alloc.deallocate(t, 1);
    }
    
    template<typename U>
    class iterator_base : public std::iterator<std::bidirectional_iterator_tag, U> {
        
        friend Tree;
        
    public:
        
        iterator_base() : _t(nullptr), _base_p(nullptr) {}
        iterator_base(const iterator_base& it) : _t(it._t), _base_p(it._base_p) {}
        
        iterator_base operator++(int) {
            
            iterator_base ret = *this;
            
            ++(*this);
            
            return ret;
        }
        
        iterator_base operator--(int) {
            
            iterator_base ret = *this;
            
            --(*this);
            
            return ret;
        }
        
        iterator_base& operator++();
        iterator_base& operator--();
        
        bool operator==(const iterator_base& it) const { return _t == it._t; }
        bool operator!=(const iterator_base& it) const { return _t != it._t; }
        
        U& operator *() { return *_t; }
        U *operator->() { return _t; }
        
        const U& operator *() const { return *_t; }
        const U *operator->() const { return _t; }
        
        operator iterator_base<const U> () {
            
            iterator_base<const U> it;
            
            it._t = _t;
            it._base_p = _base_p;
            
            return it;
        }
        
    protected:
        
        explicit iterator_base(U *t, U *base_parent) : _t(t), _base_p(base_parent) {}
    
    private:
        U *_t;
        U *_base_p;
    };
    
    Tree *_parent;
    size_type _child_id, _height;
    
    ChildrenListType _children_list;
    
    T _data;
};

template<typename T, template <typename, typename...> typename Container, typename... ContainerArgs>
class TreeOpData : public Tree<T *, Container, ContainerArgs...> {
    
    using Base = Tree<T *, Container, ContainerArgs...>;
    
public:
    
    virtual ~TreeOpData() = default;

    bool hasData() const { return Base::data() != nullptr; }
    
    void putData(const T& data) { Base::data() = new T(data); }
    void popData() { 
        
        auto& data = Base::data();
        if(data != nullptr) {
            
            delete data;
            data = nullptr;
        } 
    }
};

} /* namespace S_UTILS */

#include "tree.cpp"

#endif
