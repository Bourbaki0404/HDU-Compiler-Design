#pragma once
#include "common/common.hpp"

template<typename T> 
struct dlist_node {
    dlist_node *prev;
    dlist_node *next;

    void insert_before(dlist_node *node) {
        node->next = this;
        node->prev = this->prev;
        this->prev->next = node;
        this->prev = node;
    }

    void insert_after(dlist_node *node) {
        this->next->prev = node;
        node->next = this->next;
        node->prev = this->prev;
        this->next = node;
    }
};

// A linked-list of element of type T, where T should be a subclass of type dlist_node<T> so that we can manipulate list easily.
// T 
template<typename T>
struct dlist {
    dlist() {
        dummy_head = new nodeType;
        dummy_head->next = dummy_head;
        dummy_head->prev = dummy_head;
        _size = 0;
    }
    using nodeType = dlist_node<T>;
    using nodePtr = dlist_node<T>*;
    using pointer = T*;
    using reference = T&;
    struct iterator {
        nodePtr cur;

        iterator(nodePtr n) : cur(n) {}

        iterator() {
            cur = nullptr;
        }

        T &operator*() const { 
            return (T&)*cur; 
        }
        T *operator->() const { 
            return (T*)cur; 
        }

        iterator &operator++() {
            if (cur) cur = cur->next;
            return *this;
        }

        bool operator!=(const iterator &other) const {
            return cur != other.cur;
        }
    };

    size_t size() {
        return _size;
    }

    bool empty() {
        return _size == 0;
    }

    T *front() {
        if(!empty()) return (T*) dummy_head->next;
        else {
            std::cout << "dlist front fail\n";
            exit(1);
        }
    }

    T *back() {
        if(!empty()) return (T*) dummy_head->prev;
        else {
            std::cout << "dlist back fail\n";
            exit(1);
        }
    }

    inline void push_back(nodePtr ptr) {
        dummy_head->insert_before(ptr);
    }

    inline void push_front(nodePtr ptr) {
        dummy_head->insert_after(ptr);
    }   

    inline void insert(iterator it, nodePtr ptr) {
        insert_before(it, ptr);
    }

    inline void insert_before(iterator it, nodePtr ptr) {
        it.cur->insert_before(ptr);
    }

    inline void insert_after(iterator it, nodePtr ptr) {
        it.cur->insert_after(ptr);
    }


    iterator begin() {
        return iterator(dummy_head->next);
    }   

    iterator end() {
        return iterator(dummy_head);
    }

    size_t _size;
    nodePtr dummy_head;
};