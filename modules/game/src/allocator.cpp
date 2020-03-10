#include "allocator.h"

#include <cstdlib>

#include <stdio.h>

unsigned long int align_forward_adjustment(const void *address, unsigned int alignment)
{
    unsigned long int adjustment = alignment - ((unsigned long int)(address) & (alignment - 1));

    if (adjustment == alignment)
        return 0;

    return adjustment;
}

PoolAllocator &create_allocator(unsigned int object_size, unsigned int object_alignment, unsigned int size)
{
    PoolAllocator *allocator = (PoolAllocator *)malloc(sizeof(PoolAllocator));

    allocator->object_size = object_size;
    allocator->object_alignment = object_alignment;
    allocator->num_allocations = 0;
    allocator->used_memory = 0;

    allocator->mem = malloc(size * 10);

    if (allocator->mem == nullptr)
    {
        fprintf(stderr, "Error allocation failed\n");
    }

    unsigned long int adjustment = align_forward_adjustment(allocator->mem, object_alignment);

    allocator->free_list = (void **)((unsigned long int)(allocator->mem) + adjustment);
    void **p = allocator->free_list;

    unsigned int num_objects = (size - adjustment) / object_size;

    if (num_objects > 1000000)
    {
        fprintf(stderr, "Allocator might behave strangely with an object count of more than 1000000\n");
    }

    for (unsigned int i = 0; i < num_objects; i++)
    {
        *p = (void *)(p + object_size);
        p = (void **)*p;
    }

    *p = nullptr;

    return *allocator;
}

void delete_allocator(PoolAllocator &allocator)
{
    //TODO: delete the actual allocator as well...

    free(allocator.mem);
    // free(&allocator);
}

void *allocate(PoolAllocator &allocator)
{
    if (allocator.free_list == nullptr)
        return nullptr;

    void *p = allocator.free_list;

    allocator.free_list = (void **)(*allocator.free_list);

    allocator.used_memory += allocator.object_size;
    allocator.num_allocations++;
    return p;
}

void deallocate(PoolAllocator &allocator, void *p)
{
    if (p == nullptr)
        return;
    
    *((void **)p) = allocator.free_list;
    allocator.free_list = (void **)p;
    allocator.used_memory -= allocator.object_size;
    allocator.num_allocations--;
}
