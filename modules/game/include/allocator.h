#pragma once

// #include "types.h"

struct PoolAllocator
{
    unsigned int object_size;
    unsigned int object_alignment;
    
    void** free_list;
    void* mem;

    unsigned int used_memory;
    unsigned int num_allocations;
};

unsigned long int align_forward_adjustment(const void* address, unsigned int alignment);

PoolAllocator& create_allocator(unsigned int object_size, unsigned int object_alignment, unsigned int size);
void delete_allocator(PoolAllocator& allocator);

void* allocate(PoolAllocator& allocator);
void deallocate(PoolAllocator& allocator, void* p);
