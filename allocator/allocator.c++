#include "allocator.h"

static const long sentinels_size = sizeof(long) << 1;

/* Constructor */
template<typename T, long N>
allocator<T,N>::allocator() :
    free_bytes(N - sizeof(long) * 2),
    largest_free_block(0)
{
    if (N < space_used(1)) {
        throw std::bad_alloc();
    }
    write_sentinel(a, free_bytes);
}

/* Public functions */
template<typename T, long N>
bool
allocator<T,N>::valid() const
{
    return true;
}

/*
 * Allocates a contiguous block of memory that has the
 * capacity to store M instances of type T and returns
 * the memory address where that block starts.
 */
template<typename T, long N>
T*
allocator<T,N>::allocate(long M)
{
    /*
     * We need to take into account the two sentinels
     * that will guard the block of memory.
     */
    long bytes_needed = M * sizeof(T);
    void* ptr = &a[0];
    T* result = NULL;
    
    if (bytes_needed > free_bytes) {
        throw std::bad_alloc();
    }

    while (within_boundaries(ptr) && !result) {
        long* l_ptr = reinterpret_cast<long*>(ptr);
        long available_bytes = *l_ptr;
        if (available_bytes < bytes_needed) {
            ptr = shift_pointer(ptr, available_bytes + sentinels_size);
        } else {
            long extra_bytes = available_bytes - bytes_needed - sentinels_size;
            result = (T*) (l_ptr + 1);
            
            /* Use whole block if there's no extra space */
            if (extra_bytes < sizeof(T)) {
                write_sentinel(ptr, -available_bytes);
                free_bytes -= available_bytes;
            } else {
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

template<typename T, long N>
void
allocator<T,N>::construct(T* ptr,
                          const T* value)
{
    new (ptr) T(value);
    assert(valid());
}

template<typename T, long N>
void
allocator<T,N>::deallocate(T* ptr)
{
    void *previous_ptr, *next_ptr;
    long block_size, prev_blk_size, next_blk_size;
    
    if (!valid_block(ptr)) {
        throw std::invalid_argument("Tried to deallocate invalid address.");
        return;
    }
    block_size = *reinterpret_cast<long*>(ptr);
    if (block_size > 0) {
        /* Memory already marked as deallocated */
        return;
    }
    block_size = abs(block_size);

    /* Coalesce previous block, if possible */
    previous_ptr = shift_pointer(ptr, -(sentinels_size >> 1));
    prev_blk_size = *reinterpret_cast<long*>(previous_ptr);
    if (valid_block(previous_ptr, true) && prev_blk_size > 0) {
        ptr = shift_pointer(ptr, -(prev_blk_size + sentinels_size));
        block_size += prev_blk_size;
    }

    /* Coalesce next block, if possible */
    next_ptr = shift_pointer(ptr, block_size + sentinels_size);
    next_blk_size = *reinterpret_cast<long*>(next_ptr);
    if (valid_block(next_ptr) && next_blk_size > 0) {
        block_size += next_blk_size;
    }

    /* Write sentinels for newly freed memory */
    write_sentinel(ptr, block_size);
    free_bytes += block_size;
}

template<typename T, long N>
void
allocator<T,N>::destroy(T* ptr)
{
    ptr->~T();
    assert(valid());
}


/* Helper functions */
template<typename T, long N>
void
allocator<T,N>::write_sentinel(void* ptr,
                               long size)
{
    long* l_ptr = reinterpret_cast<long*>(ptr);
    *l_ptr = size;
    l_ptr = shift_pointer(l_ptr, abs(size) + sizeof(long));
    *l_ptr = size;
}

template<typename T, long N>
void*
allocator<T,N>::shift_pointer(void* ptr,
                              long shift)
const
{
    char *char_ptr = (char*) ptr;
    char_ptr += shift;
    ptr = (void*) char_ptr;
    return ptr;
}

template<typename T, long N>
bool
allocator<T,N>::within_boundaries(void *ptr)
const
{
    return ptr && ptr >= &a[0] && ptr <= &a[N - 1];
}

template<typename T, long N>
bool
allocator<T,N>::valid_block(void* ptr, bool inverse)
const
{
    if (within_boundaries(ptr)) {
        long blk_size = abs(*reinterpret_cast<long*>(ptr));
        void* ptr_end = inverse ? shift_pointer(ptr, -(blk_size + sentinels_size << 1)) :
                                  shift_pointer(ptr, blk_size + sentinels_size << 1);
        return within_boundaries(ptr_end) &&
               *reinterpret_cast<long*>(ptr) ==
               *reinterpret_cast<long*>(ptr_end);
    }
    return false;
}
