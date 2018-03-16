#include <stdlib.h> /* rand */
#include <iostream> /* cout, endl */

#include "my_heap.h"

int main() {
    my_heap<int> h;

    for(int i = 0; i < 15; ++i) {
        int random_num = std::rand() % 100 + 1;
        h.push(random_num);
    }

    while(!h.empty()) {
        std::cout << h.top() << " ";
        h.pop();
    }
    std::cout << std::endl;

    return 0;
}
