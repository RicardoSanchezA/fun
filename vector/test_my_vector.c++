#include "my_vector.h"
#include <iostream>

int
main (const int argc, const char** argv)
{
    my_vector<int> v;
    
    for (unsigned i = 1; i <= 25; ++i) {
    	v.push_back(i);
    	v.print();
    }

    for (unsigned i = 1; i <= 25; ++i) {
    	v.pop_back();
    	v.print();
    }

    return 0;
}
