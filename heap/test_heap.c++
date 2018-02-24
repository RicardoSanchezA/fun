#include <iostream>

#include "my_heap.h"

using namespace std;

int main() {
    my_heap<int> h;

    for(int i = 0; i < 15; ++i) {
        int random_num = rand() % 100 + 1;
        h.push(random_num);
    }

    while(!h.empty()) {
        cout << h.top() << " ";
        h.pop();
    }
    cout << endl;

    return 0;
}
