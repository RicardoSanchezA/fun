/*
 * Simple satic memory allocator, i.e., the user must provide the the capacity
 * of the allocator at compile time. Thus, only memory from the stack is used.
 */

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
    /* Constructors & Destructors */
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
    /* Data Members */
    /*
     * 'mem' is the block of memory that the allocator will use to provide
     * memory to its user. It holds enough memory to allocate N instances of
     * type T.
     */
    char mem[N * (sizeof(T) + sentinels_size)];
    int64_t bytes_allocated; // Total number of bytes allocated for 'mem'
    int64_t block_counter; // Number of ongoing memory allocations
    int64_t used_bytes; // Number of bytes that are currently in use
    int64_t free_bytes; // Number of unused bytes left in 'mem'
    /* TODO: use a heap to keep track of the largest free block */
    int64_t largest_free_block; // Largest contiguous block in 'mem'

    /* Helper functions */
    void write_sentinel(void* ptr, int64_t value);
    void* shift_pointer(void* ptr, int64_t bytes) const;
    bool within_boundaries(void* ptr) const;
    bool valid_block(void* ptr, bool inverse = false) const;
};

/*
 *************************
 ****** Constructor ******
 *************************
 */

template<typename T, int64_t N>
allocator<T,N>::allocator() :
    bytes_allocated(N * (sizeof(T) + sentinels_size)),
    block_counter(1),
    free_bytes(bytes_allocated - sentinels_size),
    largest_free_block(free_bytes)
{
    write_sentinel(mem, free_bytes);
    if(!valid()) {
        throw std::bad_alloc();
    }
}

/*
 **************************
 **** Public functions ****
 **************************
 */

template<typename T, int64_t N>
bool
allocator<T,N>::valid()
{
    bool is_valid = true;
    void* ptr = reinterpret_cast<void*>(&mem[0]);
    printf("Verifying that the memory is still valid.\n");
    while (within_boundaries(ptr)) {
        int64_t block_size = *reinterpret_cast<int64_t*>(ptr);
        printf("  [0x%p]--(%lld-bytes)--", ptr, block_size);
        ptr = shift_pointer(ptr, abs(block_size) + sentinel_size);
        if (block_size != *reinterpret_cast<int64_t*>(ptr)) {
            printf("Broken sentinel\n");
            is_valid = false;
            break;
        }
        printf("[0x%p]\n", ptr);
        ptr = shift_pointer(ptr, sentinel_size);
    }
    return is_valid && bytes_allocated >= free_bytes +
                                          block_counter * sentinels_size;
}

/*
 * Allocates a contiguous block of memory that has the capacity to store M
 * instances of type T and returns the memory address where that block starts.
 */
template<typename T, int64_t N>
T*
allocator<T,N>::allocate(int64_t M)
{
    int64_t bytes_needed = M * sizeof(T);
    void* ptr = &mem[0];
    T* result = NULL;
    
    if (bytes_needed > free_bytes) {
        throw std::bad_alloc();
    }

    while (within_boundaries(ptr) && !result) {
        int64_t* l_ptr = reinterpret_cast<int64_t*>(ptr);
        int64_t bytes_in_block = *l_ptr;
        if (bytes_in_block < bytes_needed) {
            ptr = shift_pointer(ptr, abs(bytes_in_block) + sentinels_size);
        } else {
            int64_t extra_bytes = bytes_in_block - bytes_needed;
            /* Need to account for sentinels used for the extra space block */
            extra_bytes -= sentinels_size;
            result = reinterpret_cast<T*>(shift_pointer(ptr, sentinel_size));
            
            /* Use the whole block if there's not enough extra space */
            if (extra_bytes < sizeof(T)) {
                write_sentinel(ptr, -bytes_in_block);
                free_bytes -= bytes_in_block;
            } else {
                /* Set sentinel for newly allocated space */
                write_sentinel(ptr, -bytes_needed);
                ptr = shift_pointer(ptr, bytes_needed + sentinels_size);
                free_bytes -= bytes_needed;
                ++block_counter;
                /* Set sentinel for extra space that's still unused */
                write_sentinel(ptr, extra_bytes);
                free_bytes -= sentinels_size;
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

/*
 * Deallocates the block of memory that corresponds to the pointer that was
 * provided. The pointer provided must point to the same address that was
 * returned when the user called allocate().
 */
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
        /* Memory is already marked as deallocated */
        return;
    }
    block_size = abs(block_size);
    free_bytes += block_size;

    /* Coalesce with the previous block, if possible */
    previous_ptr = shift_pointer(ptr, -sentinel_size);
    prev_blk_size = *reinterpret_cast<int64_t*>(previous_ptr);
    if (valid_block(previous_ptr, true) && prev_blk_size > 0) {
        ptr = shift_pointer(ptr, -(prev_blk_size + sentinels_size));
        block_size += prev_blk_size + sentinels_size;
        free_bytes += sentinels_size;
        --block_counter;
    }

    /* Coalesce with the next block, if possible */
    next_ptr = shift_pointer(ptr, block_size + sentinels_size);
    next_blk_size = *reinterpret_cast<int64_t*>(next_ptr);
    if (valid_block(next_ptr) && next_blk_size > 0) {
        block_size += next_blk_size + sentinels_size;
        free_bytes += sentinels_size;
        --block_counter;
    }

    /* Write sentinels for the newly freed memory */
    write_sentinel(ptr, block_size);
    assert(valid());
}

template<typename T, int64_t N>
void
allocator<T,N>::destroy(T* ptr)
{
    ptr->~T();
    assert(valid());
}

/*
 **************************
 **** Helper functions ****
 **************************
 */

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
                              int64_t shift) const
{
    char *char_ptr = (char*) ptr;
    char_ptr += shift;
    ptr = (void*) char_ptr;
    return ptr;
}

template<typename T, int64_t N>
bool
allocator<T,N>::within_boundaries(void *ptr) const
{
    return ptr && ptr >= &mem[0] &&
           ptr <= &mem[bytes_allocated - 1];
}

template<typename T, int64_t N>
bool
allocator<T,N>::valid_block(void* ptr, bool inverse) const
{
    if (within_boundaries(ptr)) {
        int64_t blk_size = abs(*reinterpret_cast<int64_t*>(ptr));
        void* ptr_end = inverse ? shift_pointer(ptr,
                                                -(blk_size + sentinel_size)) :
                                  shift_pointer(ptr, blk_size + sentinel_size);
        return within_boundaries(ptr_end) &&
               *reinterpret_cast<int64_t*>(ptr) ==
               *reinterpret_cast<int64_t*>(ptr_end);
    }
    return false;
}

#endif /* _MY_ALLOCATOR_H_ */
