#include <deque>
#include "my_lock.h"

/* 
 * Thread-safe queue backed by a deque. Uses monitors to achive synchro-
 * nization; allows multiple readers and a single writer at a time. 
 */

template <typename T, class C = std::deque<T>>
class my_queue {
    C q;
    my_lock l;
public:
    my_queue() : q(), l()
    { }

    void push(const T& value) {
        l.w_lock();
        q.push_front(value);
        l.w_unlock();
    }

    T front ()  {
        T t;
        if(!q.empty()) {
            l.r_lock();
            t = q.front();
            l.r_unlock();
        }
        return t;
    }

    T back ()  {
        T t;
        if(!q.empty()) {
            l.r_lock();
            t = q.back();
            l.r_unlock();
        }
        return t;
    }

    void pop() {
        if(!q.empty()) {
            l.w_lock();
            q.pop_back();
            l.w_unlock();
        }
    }

    bool empty() const {
        return q.size() == 0;
    }

    int size() const {
        return q.size();
    }
    ~my_queue() {
    }
};

