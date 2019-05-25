/*
 * Simple satic memory allocator. The user must provide the the capacity
 * of the allocator at compile time. Thus, only memory from the stack is
 * used, which is useful for projects where dynamic memory use is
 * disallowed or not ideal.
 *
 * Author: Ricardo Sanchez Aguilera
 */

#ifndef _MY_ALLOCATOR_H_
#define _MY_ALLOCATOR_H_

#include <new> // bad_alloc, new
#include <cassert> // assert
#include <cmath> // abs
#include <stdexcept> // invalid_argument
#include <cstddef> // ptrdiff_t, size_t

#define EXTRA_SPACE_THRESHOLD 1

static const int64_t sentinel_size = sizeof(int64_t);
static const int64_t sentinels_size = sentinel_size << 1;

template <int64_t N>
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
    allocator(bool _verbose);
    allocator(const allocator&) = default;
    ~allocator() = default;
    allocator &operator=(const allocator&) = default;
    /* Public functions */
    void* allocate(int64_t size);
    template<typename T> void construct(T* ptr, const T& value);
    void deallocate(void* ptr);
    template<typename T> void destroy(T* ptr);

private:
    /* Data Members */
    /*
     * 'mem' is the block of memory that the allocator will use to provide
     * memory to its user. It holds enough memory to allocate N instances of
     * type T.
     */
    char mem[N];
    int64_t bytes_allocated; // Total number of bytes used by the allocator.
    int64_t blocks_count; // Number of blocks that 'mem' has been divided into.
    int64_t used_bytes; // Number of bytes that are currently in use.
    int64_t free_bytes; // Number of unused bytes left.
    bool verbose;
    /* TODO: use a heap to keep track of the largest free block */

    /* Helper functions */
    void write_sentinel(void* ptr, int64_t value);
    void* shift_pointer(void* ptr, int64_t bytes) const;
    bool within_boundaries(void* ptr) const;
    bool valid_block(void* ptr, bool inverse = false) const;
    bool valid();
};

/*
 *************************
 ****** Constructor ******
 *************************
 */

template<int64_t N>
allocator<N>::allocator() :
    bytes_allocated(N),
    blocks_count(1),
    free_bytes(bytes_allocated - sentinels_size),
    verbose(false)
{
    write_sentinel(mem, free_bytes);
    if(!valid()) {
        throw std::bad_alloc();
    }
}

template<int64_t N>
allocator<N>::allocator(bool _verbose) :
    bytes_allocated(N),
    blocks_count(1),
    free_bytes(bytes_allocated - sentinels_size),
    verbose(_verbose)
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

template<int64_t N>
bool
allocator<N>::valid()
{
    bool is_valid = true;
    void* ptr = reinterpret_cast<void*>(&mem[0]);
    if (verbose) printf("Verifying that the memory is valid.\n");
    while (within_boundaries(ptr)) {
        int64_t block_size = *reinterpret_cast<int64_t*>(ptr);
        if (verbose) printf("  [0x%p]--(%lld-bytes)--", ptr, block_size);
        ptr = shift_pointer(ptr, abs(block_size) + sentinel_size);
        if (block_size != *reinterpret_cast<int64_t*>(ptr)) {
            if (verbose) printf("Broken sentinel\n");
            is_valid = false;
            break;
        }
        if (verbose) printf("[0x%p]\n", ptr);
        ptr = shift_pointer(ptr, sentinel_size);
    }
    return is_valid && bytes_allocated >= free_bytes +
                                          blocks_count * sentinels_size;
}

/*
 * Allocates a contiguous block of memory of at least M-bytes. A pointer to the
 * beginning of the block of memory is returned. If there is not enough memory,
 * then a null pointer will be returned.
 */
template<int64_t N>
void*
allocator<N>::allocate(int64_t M)
{
    int64_t bytes_needed = M;
    void* ptr = reinterpret_cast<void*>(&mem[0]);
    void* result = NULL;
    
    if (bytes_needed > free_bytes) {
        if (verbose) printf("Not enough space for %lld bytes.\n", M);
        return NULL;
    }
    if (verbose) printf("Allocating %lld bytes.\n", M);

    while (within_boundaries(ptr) && !result) {
        int64_t* l_ptr = reinterpret_cast<int64_t*>(ptr);
        int64_t bytes_in_block = *l_ptr;
        if (bytes_in_block < bytes_needed) {
            ptr = shift_pointer(ptr, abs(bytes_in_block) + sentinels_size);
        } else {
            int64_t extra_bytes = bytes_in_block - bytes_needed;
            /* Need to account for sentinels used for the extra space block */
            extra_bytes -= sentinels_size;
            result = shift_pointer(ptr, sentinel_size);
            
            /* Use the whole block if there's not enough extra space */
            if (extra_bytes < EXTRA_SPACE_THRESHOLD) {
                write_sentinel(ptr, -bytes_in_block);
                free_bytes -= bytes_in_block;
            } else {
                /* Set sentinel for newly allocated space */
                write_sentinel(ptr, -bytes_needed);
                ptr = shift_pointer(ptr, bytes_needed + sentinels_size);
                free_bytes -= bytes_needed;
                ++blocks_count;
                /* Set sentinel for extra space that's still unused */
                write_sentinel(ptr, extra_bytes);
                free_bytes -= sentinels_size;
            }
        }
    }

    assert(valid());
    return result;
}

template<int64_t N>
template<typename T>
void
allocator<N>::
construct(T* ptr,
          const T& value)
{
    T* t_ptr = reinterpret_cast<T*>(ptr);
    new (t_ptr) T(value);
    assert(valid());
}

/*
 * Releases the memory used by the block that starts at the given pointer.
 * The pointer provided must point to the same address that was returned by
 * allocate().
 */
template<int64_t N>
void
allocator<N>::deallocate(void* p)
{
    void *ptr, *previous_ptr, *next_ptr;
    int64_t block_size, prev_blk_size, next_blk_size;

    ptr = p;
    ptr = shift_pointer(ptr, -sentinel_size);
    
    if (!valid_block(ptr)) {
        if (verbose) printf("Tried to deallocate an invalid address.\n");
        throw std::invalid_argument("Tried to deallocate invalid address.");
        return;
    }
    block_size = *reinterpret_cast<int64_t*>(ptr);
    if (block_size > 0) {
        /* Memory is already marked as deallocated */
        if (verbose) printf("Tried to deallocate an address that is already "
                            "marked as deallocated.\n");
        return;
    }
    block_size = abs(block_size);
    if (verbose) printf("Deallocating %lld bytes.\n", block_size);
    free_bytes += block_size;

    /* Coalesce with the previous block, if possible */
    previous_ptr = shift_pointer(ptr, -sentinel_size);
    prev_blk_size = *reinterpret_cast<int64_t*>(previous_ptr);
    if (valid_block(previous_ptr, true) && prev_blk_size > 0) {
        ptr = shift_pointer(ptr, -(prev_blk_size + sentinels_size));
        block_size += prev_blk_size + sentinels_size;
        free_bytes += sentinels_size;
        --blocks_count;
    }

    /* Coalesce with the next block, if possible */
    next_ptr = shift_pointer(ptr, block_size + sentinels_size);
    next_blk_size = *reinterpret_cast<int64_t*>(next_ptr);
    if (valid_block(next_ptr) && next_blk_size > 0) {
        block_size += next_blk_size + sentinels_size;
        free_bytes += sentinels_size;
        --blocks_count;
    }

    /* Write sentinels for the newly freed memory */
    write_sentinel(ptr, block_size);
    assert(valid());
}

template<int64_t N>
template<typename T>
void
allocator<N>::destroy(T* ptr)
{
    ptr->~T();
    assert(valid());
}

/*
 **************************
 **** Helper functions ****
 **************************
 */

template<int64_t N>
void
allocator<N>::write_sentinel(void* ptr,
                               int64_t size)
{
    int64_t* l_ptr = reinterpret_cast<int64_t*>(ptr);
    *l_ptr = size;
    ptr = shift_pointer(ptr, abs(size) + sizeof(int64_t));
    l_ptr = reinterpret_cast<int64_t*>(ptr);
    *l_ptr = size;
}

template<int64_t N>
void*
allocator<N>::shift_pointer(void* ptr,
                              int64_t shift) const
{
    char *char_ptr = (char*) ptr;
    char_ptr += shift;
    ptr = (void*) char_ptr;
    return ptr;
}

template<int64_t N>
bool
allocator<N>::within_boundaries(void *ptr) const
{
    return ptr && ptr >= &mem[0] &&
           ptr <= &mem[bytes_allocated - 1];
}

template<int64_t N>
bool
allocator<N>::valid_block(void* ptr, bool inverse) const
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
