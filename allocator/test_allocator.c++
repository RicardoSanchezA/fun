#include <iostream>
#include <vector>
#include "allocator.h"

typedef struct my_struct_s {
    int datum;
} my_struct_t;

int
main (const int argc, const char** argv)
{
    // Test allocator with 99-bytes of capacity
    allocator<99> a(true);

    std::vector<my_struct_t *> v;
    std::vector<int> nums;
    my_struct_t *ptr = NULL;
    int i = 0;

    while ((ptr = reinterpret_cast<my_struct_t *>(a.allocate(sizeof(my_struct_t))))) {
        ptr->datum = ++i;
        v.push_back(ptr);
        nums.push_back(i);
    }

    auto nums_it = nums.begin();
    for (auto it = v.begin(); it != v.end(); ++it, ++nums_it) {
        if (*nums_it != (*it)->datum) {
            break;
        }
        a.deallocate(*it);
    }
    if (nums_it != nums.end()) {
        std::cout << "Something went wrong...\n";
    } else {
        std::cout << "Test passed.\n";
    }

    return 0;
}
