#ifndef _MY_LOCK_H_
#define _MY_LOCK_H_

#include <pthread.h>

struct my_lock {
    pthread_cond_t readers_cv, writers_cv;
    pthread_mutex_t lock;
    int num_readers, num_writers;
    my_lock() : readers_cv(), writers_cv(), lock(), 
                num_readers(0), num_writers(0)
    {
        pthread_cond_init(&readers_cv, NULL);
        pthread_cond_init(&writers_cv, NULL);
        pthread_mutex_init(&lock, NULL);
    }

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
    ~my_lock() {
        pthread_cond_destroy(&readers_cv);
        pthread_cond_destroy(&writers_cv);
        pthread_mutex_destroy(&lock);
    }
};

#endif /* _MY_LOCK_H_ */
