#ifndef _MY_ALLOCATOR_H_
#define _MY_ALLOCATOR_H_

#include <new> // bad_alloc, new
#include <cassert> // assert
#include <cmath> // abs
#include <stdexcept> // invalid_argument
#include <cstddef> // ptrdiff_t, size_t


template <typename T, long N>
class allocator {
public:
    friend bool operator==(const allocator&, const allocator&) {
        return false;
    }
    friend bool operator!=(const allocator& lhs, const allocator& rhs) {
        return !(lhs == rhs);
    }
    /* Constructor */
    allocator();
    allocator(const allocator&) = default;
    ~allocator() = default;
    allocator &operator=(const allocator&) = default;
    /* Public functions */
    T* allocate(long size);
    void construct(T* ptr, const T* value);
    void deallocate(T* ptr);
    void destroy(T* ptr);
    bool valid() const;

private:
    /* Data */
    char a[N]; // Block of memory of N bytes
    long free_bytes; // Total amount of bytes left in 'a'
    long largest_free_block; // Largest contiguous block in 'a'

    /* Helper functions */
    void write_sentinel(void* ptr, long value);
    void* shift_pointer(void* ptr, long bytes) const;
    long space_used(long M) { return sizeof(T) * M + sizeof(long) * 2; }
    bool within_boundaries(void* ptr) const;
    bool valid_block(void* ptr, bool inverse = false) const;
};

#endif /* _MY_ALLOCATOR_H_ */
