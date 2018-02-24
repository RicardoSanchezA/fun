#include <iostream> /* printf */
#include <algorithm> /* swap */
#include <vector> /* vector */
#include <stdlib.h> /* rand */

#define HEAP_MAX_SIZE 100

using namespace std;


template<typename T>
struct Compare { 
    bool operator()(T lhs, T rhs) {
        return lhs < rhs;
    }
};

template<typename T = int, typename C = Compare<T> >
class my_heap {
    C cmp;
    long size;
    vector<T> heap;

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
        if(l < size && cmp(heap[l], heap[smallest])) {
            smallest = l;
        }
        if(r < size && cmp(heap[r], heap[smallest])) {
            smallest = r;
        }
        if(smallest != root) {
            swap(heap[smallest], heap[root]);
            heapify(smallest);
        }
    }

public:
    my_heap() : size(0), heap(vector<T>(HEAP_MAX_SIZE)) {}

    void push(const T& data) {
        if(size < heap.size() - 1) {
            heap[size] = data;
            int i = size;
            int parent_idx = parent(i);
            while(parent_idx >= 0 && heap[i] < heap[parent_idx]) {
                swap(heap[i], heap[parent_idx]);
                i = parent_idx;
                parent_idx = parent(i);
            }
            ++size;
        }
    }

    T top() const{
        int t = -1;
        if(size > 0)
            t = heap[0];
        return t;
    }

    void pop() {
        if(size > 0) {
            heap[0] = heap[size - 1];
            --size;
            heapify(0);
        }
    }

    void delete_at(int idx) {
        heap[idx] = heap[0];
        while(idx > 0 && cmp(heap[idx], heap[parent(idx)])) {
            swap(heap[idx], heap[parent(idx)]);
            --idx;
        }
        pop();
    }

    bool empty() const {
        return size == 0;
    }

};