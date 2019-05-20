#include <iostream>
#include "allocator.h"

int
main (const int argc, const char** argv)
{
    allocator<int, 5> a;
    int b, c, d;
    b = 5; c = 10; d = 15;
    int *ptr1, *ptr2;

    ptr1 = a.allocate(1);
    a.construct(ptr1, b);
    if (*ptr1 == b) {
    	std::cout << "equal\n";
    } else {
    	std::cout << "not equal\n";
    }


    ptr2 = a.allocate(2);
    a.construct(ptr2, c);
    a.construct(ptr2 + 1, d);
    if (*ptr2 == c && *(ptr2+1) == d) {
    	std::cout << "equal\n";
    } else {
    	std::cout << "not equal\n";
    }

    a.destroy(ptr1);
    a.deallocate(ptr1);

    a.destroy(ptr2);
    a.deallocate(ptr2);

    return 0;
}
