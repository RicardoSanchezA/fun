#include <iostream>
#include <stdlib.h>
#include "my_queue.h"

#define NUM_THREADS 30
#define NUMS_PER_THREAD 100

/* Test: consists of pushing NUMS_PER_THREADS numbers into
 * the queue and then pop all of them except the last one.
 * We make dummy calls to front() and back() just to invoke
 * many readings to the queue and test its responsiveness
 * to many threads). */
void *run_test(void* _q) {
	my_queue<int>* q = ((my_queue<int>*)_q);
	
	int counter = std::rand() % 100 + 1;
	for(int i = 0; i < NUMS_PER_THREAD; ++i, ++counter) {
		q->push(counter);
	}

	for(int i = 0; i < NUMS_PER_THREAD; ++i) {
		q->front();
		if(i < NUMS_PER_THREAD - 1) {
			q->pop();
		}
		q->back();
	}
}

int main() {

	pthread_t threads[NUM_THREADS];
	my_queue<int> q;
	int rc;
	void* status;

	for(int i = 0; i < NUM_THREADS; ++i) {
		rc = pthread_create(&threads[i], NULL, run_test, (void *)&q);
	}

	for(int i = 0; i < NUM_THREADS; ++i) {
		rc = pthread_join(threads[i], &status);
	}

	std::cout << "Queue size: " << q.size() << " ... ";
	std::cout << "front: " << q.front() << " , back: " << q.back() << "\n";

	return 0;
}
