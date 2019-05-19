#ifndef _MY_ALLOCATOR_H_
#define _MY_ALLOCATOR_H_

#include <new> // bad_alloc, new
#include <cassert> // assert
#include <cmath> // abs
#include <stdexcept> // invalid_argument
#include <cstddef> // ptrdiff_t, size_t

static const int64_t sentinel_size = sizeof(int64_t);
static const int64_t sentinels_size = sentinel_size << 1;

template <typename T, int64_t N>
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
    T* allocate(int64_t size);
    void construct(T* ptr, const T& value);
    void deallocate(T* ptr);
    void destroy(T* ptr);
    bool valid();

private:
    /* Data */
    char a[N * sizeof(T) + N * sentinels_size]; // Block of memory of N bytes
    int64_t bytes_allocated; // Total amounts of bytes allocated
    int64_t free_bytes; // Total amount of bytes left in 'a'
    int64_t largest_free_block; // Largest contiguous block in 'a'

    /* Helper functions */
    void write_sentinel(void* ptr, int64_t value);
    void* shift_pointer(void* ptr, int64_t bytes) const;
    int64_t space_used(int64_t M) { return sizeof(T) * M + sizeof(int64_t) * 2; }
    bool within_boundaries(void* ptr) const;
    bool valid_block(void* ptr, bool inverse = false) const;
};

/* Constructor */
template<typename T, int64_t N>
allocator<T,N>::allocator() :
    bytes_allocated(N * sizeof(T) + N * sentinels_size),
    free_bytes(bytes_allocated - sentinels_size),
    largest_free_block(0)
{
    if (bytes_allocated < space_used(1)) {
        throw std::bad_alloc();
    }
    write_sentinel(a, free_bytes);
}

/* Public functions */
template<typename T, int64_t N>
bool
allocator<T,N>::valid()
{
    bool is_valid = true;
    void* ptr = reinterpret_cast<void*>(&a[0]);
    printf("Checking memory is not corrupt\n");
    while (within_boundaries(ptr)) {
        int64_t block_size = *reinterpret_cast<int64_t*>(ptr);
        printf("  [0x%x]--(%ld-bytes)--", ptr, block_size);
        ptr = shift_pointer(ptr, abs(block_size) + sentinel_size);
        if (block_size != *reinterpret_cast<int64_t*>(ptr)) {
            printf("Broken sentinel\n");
            is_valid = false;
            break;
        }
        printf("[0x%x]\n", ptr);
        ptr = shift_pointer(ptr, sentinel_size);
    }
    printf("\n");
    return is_valid;
}

/*
 * Allocates a contiguous block of memory that has the
 * capacity to store M instances of type T and returns
 * the memory address where that block starts.
 */
template<typename T, int64_t N>
T*
allocator<T,N>::allocate(int64_t M)
{
    /*
     * We need to take into account the two sentinels
     * that will guard the block of memory.
     */
    int64_t bytes_needed = M * sizeof(T);
    void* ptr = &a[0];
    T* result = NULL;
    
    if (bytes_needed > free_bytes) {
        throw std::bad_alloc();
    }

    while (within_boundaries(ptr) && !result) {
        //printf("->ptr: 0x%x\n" , ptr);
        int64_t* l_ptr = reinterpret_cast<int64_t*>(ptr);
        int64_t available_bytes = *l_ptr;
        if (available_bytes < bytes_needed) {
            ptr = shift_pointer(ptr, abs(available_bytes) + sentinels_size);
        } else {
            int64_t extra_bytes = available_bytes - bytes_needed - sentinels_size;
            result = reinterpret_cast<T*>(shift_pointer(ptr, sentinel_size));
            
            /* Use whole block if there's no extra space */
            if (extra_bytes < sizeof(T)) {
                //printf("allocated whole block\n");
                write_sentinel(ptr, -available_bytes);
                free_bytes -= available_bytes;
            } else {
                //printf("allocated %ld with %ld remaining\n", bytes_needed, extra_bytes);
                write_sentinel(ptr, -bytes_needed);
                ptr = shift_pointer(ptr, bytes_needed + sentinels_size);
                write_sentinel(ptr, extra_bytes);
                free_bytes -= bytes_needed;
            }
        }
    }

    assert(valid());
    return result;
}

template<typename T, int64_t N>
void
allocator<T,N>::construct(T* ptr,
                          const T& value)
{
    new (ptr) T(value);
    assert(valid());
}

template<typename T, int64_t N>
void
allocator<T,N>::deallocate(T* p)
{
    void *ptr, *previous_ptr, *next_ptr;
    int64_t block_size, prev_blk_size, next_blk_size;

    ptr = reinterpret_cast<void*>(p);
    ptr = shift_pointer(ptr, -sentinel_size);
    
    if (!valid_block(ptr)) {
        throw std::invalid_argument("Tried to deallocate invalid address.");
        return;
    }
    block_size = *reinterpret_cast<int64_t*>(ptr);
    if (block_size > 0) {
        /* Memory already marked as deallocated */
        return;
    }
    block_size = abs(block_size);

    /* Coalesce previous block, if possible */
    previous_ptr = shift_pointer(ptr, -sentinel_size);
    prev_blk_size = *reinterpret_cast<int64_t*>(previous_ptr);
    if (valid_block(previous_ptr, true) && prev_blk_size > 0) {
        ptr = shift_pointer(ptr, -(prev_blk_size + sentinels_size));
        block_size += prev_blk_size + sentinels_size;
    }

    /* Coalesce next block, if possible */
    next_ptr = shift_pointer(ptr, block_size + sentinels_size);
    next_blk_size = *reinterpret_cast<int64_t*>(next_ptr);
    if (valid_block(next_ptr) && next_blk_size > 0) {
        block_size += next_blk_size + sentinels_size;
    }

    /* Write sentinels for newly freed memory */
    write_sentinel(ptr, block_size);
    free_bytes += block_size;
    assert(valid());
}

template<typename T, int64_t N>
void
allocator<T,N>::destroy(T* ptr)
{
    ptr->~T();
    assert(valid());
}


/* Helper functions */
template<typename T, int64_t N>
void
allocator<T,N>::write_sentinel(void* ptr,
                               int64_t size)
{
    int64_t* l_ptr = reinterpret_cast<int64_t*>(ptr);
    *l_ptr = size;
    ptr = shift_pointer(ptr, abs(size) + sizeof(int64_t));
    l_ptr = reinterpret_cast<int64_t*>(ptr);
    *l_ptr = size;
}

template<typename T, int64_t N>
void*
allocator<T,N>::shift_pointer(void* ptr,
                              int64_t shift)
const
{
    char *char_ptr = (char*) ptr;
    char_ptr += shift;
    ptr = (void*) char_ptr;
    return ptr;
}

template<typename T, int64_t N>
bool
allocator<T,N>::within_boundaries(void *ptr)
const
{
    return ptr && ptr >= &a[0] &&
           ptr <= &a[bytes_allocated - 1];
}

template<typename T, int64_t N>
bool
allocator<T,N>::valid_block(void* ptr, bool inverse)
const
{
    if (within_boundaries(ptr)) {
        int64_t blk_size = abs(*reinterpret_cast<int64_t*>(ptr));
        void* ptr_end = inverse ? shift_pointer(ptr, -(blk_size + sentinel_size)) :
                                  shift_pointer(ptr, blk_size + sentinel_size);
        return within_boundaries(ptr_end) &&
               *reinterpret_cast<int64_t*>(ptr) ==
               *reinterpret_cast<int64_t*>(ptr_end);
    }
    return false;
}

#endif /* _MY_ALLOCATOR_H_ */
