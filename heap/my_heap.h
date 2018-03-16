#include <iostream> /* printf */
#include <algorithm> /* swap */
#include <vector> /* vector */
#include <stdlib.h> /* rand */

#define HEAP_MAX_SIZE 300


template<typename T>
struct Compare { 
    bool operator()(T lhs, T rhs) {
        return lhs < rhs;
    }
};

template<typename T = int, typename C = Compare<T> >
class my_heap {
    C cmp;
    long _size;
    std::vector<T> heap;

    const int parent(const int& idx) {
        return (idx - 1)/2;
    }
    const int left(const int& idx) {
        return 2*idx + 1;
    }
    const int right(const int& idx) {
        return 2*idx + 2;
    }
    void heapify(const int& root) {
        int l = left(root);
        int r = right(root);
        int smallest = root;
        if(l < _size && cmp(heap[l], heap[smallest])) {
            smallest = l;
        }
        if(r < _size && cmp(heap[r], heap[smallest])) {
            smallest = r;
        }
        if(smallest != root) {
            std::swap(heap[smallest], heap[root]);
            heapify(smallest);
        }
    }

public:
    my_heap() : _size(0), heap(std::vector<T>(HEAP_MAX_SIZE)) {}

    void push(const T& data) {
        if(_size < heap.size() - 1) {
            heap[_size] = data;
            int i = _size;
            int parent_idx = parent(i);
            while(parent_idx >= 0 && cmp(heap[i], heap[parent_idx])) {
                std::swap(heap[i], heap[parent_idx]);
                i = parent_idx;
                parent_idx = parent(i);
            }
            ++_size;
        }
    }

    T top() const{
        T t;
        if(_size > 0)
            t = heap[0];
        return t;
    }

    void pop() {
        if(_size > 0) {
            heap[0] = heap[_size - 1];
            --_size;
            heapify(0);
        }
    }

    void delete_at(int idx) {
        heap[idx] = heap[0];
        while(idx > 0 && cmp(heap[idx], heap[parent(idx)])) {
            std::swap(heap[idx], heap[parent(idx)]);
            --idx;
        }
        pop();
    }

    bool empty() const {
        return _size == 0;
    }

    int size() const {
        return _size;
    }
};