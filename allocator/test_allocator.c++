#include "allocator.h"

int
main (const int argc, const char** argv)
{
    allocator<int, 1000> a;
    return 0;
}
