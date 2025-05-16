#pragma once
#include "common/common.hpp"


template<typename T> 
struct dlist_node;


// A doubly linked intrusive list node implementation
// If T is the node of the list, then it must inherit from dlist_node<T>
template<typename T> 
struct dlist_node {
    template <typename, typename, typename> friend struct dlist;

    dlist_node *prev = nullptr;
    dlist_node *next = nullptr;

    using nodeType = dlist_node<T>;
    using nodePtr = dlist_node<T>*;

    struct iterator {
        using value_type = T;
        using difference_type = uint64_t;
        using pointer = T*;
        using reference = T&;
        
        nodePtr cur;

        iterator(nodePtr n) : cur(n) {}
        iterator() : cur(nullptr) {}

        reference operator*() const { return static_cast<reference>(*cur); }
        pointer operator->() const { return static_cast<pointer>(cur); }

        iterator &operator++() {
            cur = cur->next;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool atEnd() const {
            return cur->isDummy();
        }

        bool operator!=(const iterator &other) const { return cur != other.cur; }
        bool operator==(const iterator &other) const { return cur == other.cur; }
    };

    struct const_iterator {
        using value_type = const T;
        using difference_type = uint64_t;
        using pointer = const T*;
        using reference = const T&;

        const dlist_node<T> *cur;
        const_iterator(const nodePtr n) : cur(n) {}
        const_iterator() : cur(nullptr) {}
        const_iterator &operator++() {
            cur = cur->next;
            return *this;
        }

        const_iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool atEnd() const {
            return cur->isDummy();
        }

        reference operator*() const { return (reference)(*cur); }
        pointer operator->() const { return (pointer)(cur); }

        bool operator!=(const const_iterator &other) const { return cur != other.cur; }
        bool operator==(const const_iterator &other) const { return cur == other.cur; }
    };

    struct reverse_iterator {
        using value_type = T;
        using difference_type = uint64_t;
        using pointer = T*;
        using reference = T&;

        nodePtr cur;

        reverse_iterator(nodePtr n) : cur(n) {}
        reverse_iterator() : cur(nullptr) {}

        reference operator*() const { return static_cast<reference>(*cur); }
        pointer operator->() const { return static_cast<pointer>(cur); }

        reverse_iterator &operator++() {
            cur = cur->prev;
            return *this;
        }

        reverse_iterator operator++(int) {
            reverse_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool atEnd() const {
            return cur->isDummy();
        }

        bool operator!=(const reverse_iterator &other) const { return cur != other.cur; }
        bool operator==(const reverse_iterator &other) const { return cur == other.cur; }
    };

    struct const_reverse_iterator {
        using value_type = const T;
        using difference_type = uint64_t;
        using pointer = const T*;
        using reference = const T&;

        const nodePtr cur;

        const_reverse_iterator(nodePtr n) : cur(n) {}
        const_reverse_iterator() : cur(nullptr) {}

        reference operator*() const { return static_cast<reference>(*cur); }
        pointer operator->() const { return static_cast<pointer>(cur); }

        const_reverse_iterator &operator++() {
            cur = cur->prev;
            return *this;
        }

        const_reverse_iterator operator++(int) {
            const_reverse_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool atEnd() const {
            return cur->isDummy();
        }

        bool operator!=(const const_reverse_iterator &other) const { return cur != other.cur; }
        bool operator==(const const_reverse_iterator &other) const { return cur == other.cur; }
    };

    iterator getIterator() { return iterator(this); }

    void setIsDummy(bool isDummy) {
        this->isDummy_ = isDummy;
    }

    bool isDummy() const {
        return isDummy_;
    }

protected:
    // this method can only be used in class dlist
    void insert_before(dlist_node *node) {
        node->next = this;
        node->prev = prev;
        if (prev) prev->next = node;
        prev = node;
    }

    void insert_after(dlist_node *node) {
        node->prev = this;
        node->next = next;
        if (next) next->prev = node;
        next = node;
    }
    
    void remove_self() {
        if (prev) prev->next = next;
        if (next) next->prev = prev;
        prev = nullptr;
        next = nullptr;
    }
private:

    // To indicate if the node is at end of the list
    bool isDummy_ = false;
};

template <typename T>
struct dlist_callback_traits {
    using NodeTy = dlist_node<T>;
    void addNodeToList(NodeTy *) {}
    void removeNodeFromList(NodeTy *) {}

    template <typename iterator>
    void transferNodesFromList(dlist_callback_traits &, iterator, iterator) {}
};

template<typename Traits, typename T>
struct is_valid_traits : std::is_base_of<dlist_callback_traits<T>, Traits> {};


template<typename T, typename Traits = dlist_callback_traits<T>, 
         typename = std::enable_if_t<is_valid_traits<Traits, T>::value>>
struct dlist : public Traits {
    dlist() {
        dummy_head = new nodeType;
        dummy_head->next = dummy_head;
        dummy_head->prev = dummy_head;
        dummy_head->setIsDummy(true);
        _size = 0;
    }
    
    // ~dlist() {
    //     clear();
    //     delete dummy_head;
    // }
    

    // Disable copy operations
    dlist(const dlist&) = delete;
    dlist& operator=(const dlist&) = delete;
    using nodeType = dlist_node<T>;
    using nodePtr = dlist_node<T>*;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = typename dlist_node<T>::iterator;
    using const_iterator = typename dlist_node<T>::const_iterator;
    using reverse_iterator = typename dlist_node<T>::reverse_iterator;
	using const_reverse_iterator = typename dlist_node<T>::const_reverse_iterator;
    using Traits::addNodeToList;
    using Traits::removeNodeFromList;

    size_t size() const { return _size; }
    bool empty() const { return _size == 0; }

    reference front() {
        if (empty()) {
            std::cout << "dlist front fail\n";
            exit(1);
        }
        return *begin();
    }

    const_reference front() const {
        if(empty()) {
            std::cout << "dlist front fail\n";
            exit(1);            
        }
        return *begin();
    }


    reference back() {
        if (empty()) {
            std::cout << "dlist back fail\n";
        }
        return *iterator(dummy_head->prev);
    }

    const_reference back() const {
        if (empty()) {
            std::cout << "dlist back fail\n";
        }
        return *iterator(dummy_head->prev);
    }

    void push_front(pointer val) { insert(begin(), val); }
    void push_back(pointer val) { insert(end(), val); }
    
    iterator insertAfter(iterator where, pointer New) {
        if (empty())
            return insert(begin(), New);
        else
            return insert(++where, New);
    }

    iterator insert(iterator pos, pointer node) {
        if (!node) return pos;
        
        // Remove node from its current list if it's in one
        if (node->next || node->prev) {
            std::cout << "dlist insert fail. The node is already on list.\n";
            exit(1);
        }
        
        pos.cur->insert_before(node);
        _size++;

        // callback function
        addNodeToList(node);
        return iterator(node);
    }

    iterator erase(iterator pos) {
        if (pos == end()) return pos;
        
        iterator next = pos;
        ++next;
        // remove_node(pos.cur);
        return next;
    }


    pointer remove(const iterator &IT) {
        iterator MutIt = IT;
        return remove(MutIt);
    }
    
    pointer remove(iterator &IT) {
        pointer Node = &*IT++;
        removeNodeFromList(Node); // Notify traits that we removed a node...
        Node->remove_self();
        return Node;
    }

    void clear() {

    }
    
    // iterators
    iterator begin() { return iterator(dummy_head->next); }
    const_iterator begin() const { return const_iterator(dummy_head->next); }
    iterator end() { return iterator(dummy_head); }
    const_iterator end() const { return const_iterator(dummy_head); }
    // reverse iterators
    reverse_iterator rbegin() { return reverse_iterator(dummy_head->prev); }
    reverse_iterator rend() { return reverse_iterator(dummy_head); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(dummy_head->prev); }
    const_reverse_iterator rend() const { return const_reverse_iterator(dummy_head); }
 
private:

    size_t _size;
    nodePtr dummy_head;
};