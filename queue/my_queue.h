#include <pthread.h>
#include <deque>

/* Thread-safe queue backed by a deque. Uses monitors to achive synchro-
 * nization; allows multiple readers and a single writer at a time. */
template <typename T, class C = std::deque<T>>
class my_queue {
    C q;
    pthread_cond_t readers_cv, writers_cv;
    pthread_mutex_t lock;
    int num_readers, num_writers;

    void w_lock() {
        pthread_mutex_lock(&lock);
        ++num_writers;
        if(num_writers > 1 || num_readers > 0) {
            pthread_cond_wait(&writers_cv, &lock);
        }
        pthread_mutex_unlock(&lock);
    }
    void w_unlock() {
        pthread_mutex_lock(&lock);
        --num_writers;
        if(num_writers > 0) {
            pthread_cond_signal(&writers_cv);
        }
        pthread_cond_broadcast(&readers_cv);
        pthread_mutex_unlock(&lock);
    }

    void r_lock() {
        pthread_mutex_lock(&lock);
        while(num_writers > 0) {
            pthread_cond_wait(&readers_cv, &lock);
        }
        ++num_readers;
        pthread_mutex_unlock(&lock);
    }
    void r_unlock() {
        pthread_mutex_lock(&lock);
        --num_readers;
        if(num_readers == 0 && num_writers > 0) {
            pthread_cond_signal(&writers_cv);
        }
        pthread_mutex_unlock(&lock);
    }

public:
    my_queue() : q(), readers_cv(), writers_cv(), lock(), 
        num_readers(0), num_writers(0) {
            pthread_cond_init(&readers_cv, NULL);
            pthread_cond_init(&writers_cv, NULL);
            pthread_mutex_init(&lock, NULL);
        }

    void push(const T& value) {
        w_lock();
        q.push_front(value);
        w_unlock();
    }

    T front ()  {
        T t;
        if(!q.empty()) {
            r_lock();
            t = q.front();
            r_unlock();
        }
        return t;
    }

    T back ()  {
        T t;
        if(!q.empty()) {
            r_lock();
            t = q.back();
            r_unlock();
        }
        return t;
    }

    void pop() {
        if(!q.empty()) {
            w_lock();
            q.pop_back();
            w_unlock();
        }
    }

    bool empty() const {
        return q.size() == 0;
    }

    int size() const {
        return q.size();
    }
    ~my_queue() {
        pthread_cond_destroy(&readers_cv);
        pthread_cond_destroy(&writers_cv);
        pthread_mutex_destroy(&lock);
    }
};

