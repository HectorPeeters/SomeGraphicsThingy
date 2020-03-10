#include "allocator_test.h"

#include "test.h"

#include "allocator.h"

#include <stdio.h>

namespace test
{

struct TestObject
{
    bool flag = true;
    int amount = 123;
};

void test_simple_allocation()
{
    PoolAllocator allocator = create_allocator(sizeof(TestObject), sizeof(TestObject), sizeof(TestObject));

    ASSERT_EQ(allocator.object_size, sizeof(TestObject), "The object size of the allocator should equal the size of the struct")

    TestObject *object = (TestObject *)allocate(allocator);
    ASSERT_NEQ(object, nullptr, "Allocated object should not be null");
    object->amount = 321;
    object->flag = false;

    ASSERT_EQ(allocator.num_allocations, 1, "Only one allocation should have been made")
    ASSERT_EQ(allocator.used_memory, sizeof(TestObject), "The used memory should be equal to the size of the struct")

    deallocate(allocator, object);

    ASSERT_EQ(allocator.num_allocations, 0, "There shouldn't be any allocation left in the allocator")
    ASSERT_EQ(allocator.used_memory, 0, "The used memory should be 0 after deallocating everything")

    delete_allocator(allocator);
}

void test_multiple_single_allocation()
{
    const int allocation_count = 20000000;

    PoolAllocator allocator = create_allocator(sizeof(TestObject), sizeof(TestObject), sizeof(TestObject) * 2);

    for (int i = 0; i < allocation_count; i++)
    {
        TestObject *object = (TestObject *)allocate(allocator);
        ASSERT_NEQ(object, nullptr, "Allocated object should not be null");

        object->amount = 321;
        object->flag = false;

        ASSERT_EQ(allocator.num_allocations, 1, "Only one allocation should have been made")
        ASSERT_EQ(allocator.used_memory, sizeof(TestObject), "The used memory should be equal to the size of the struct")

        deallocate(allocator, object);

        ASSERT_EQ(allocator.num_allocations, 0, "There shouldn't be any allocation left in the allocator")
        ASSERT_EQ(allocator.used_memory, 0, "The used memory should be 0 after deallocating everything")
    }
}

void test_multiple_allocation()
{
    const int allocation_count = 1000000;

    PoolAllocator allocator = create_allocator(sizeof(TestObject), sizeof(TestObject), sizeof(TestObject) * allocation_count);

    TestObject *pointers[allocation_count];

    for (int i = 0; i < allocation_count; i++)
    {
        TestObject *object = (TestObject *)allocate(allocator);

        pointers[i] = object;

        ASSERT_NEQ(pointers[i], nullptr, "Allocated object should not be null");

        pointers[i]->amount = 321;
        pointers[i]->flag = false;

        ASSERT_EQ(allocator.num_allocations, i + 1, "Only one allocation should have been made")
        ASSERT_EQ(allocator.used_memory, (i + 1) * sizeof(TestObject), "The used memory should be equal to the size of the struct")
    }

    for (int i = allocation_count - 1; i > 0; i--)
    {
        deallocate(allocator, pointers[i]);

        ASSERT_EQ(allocator.num_allocations, i, "Only one allocation should have been made")
        ASSERT_EQ(allocator.used_memory, i * sizeof(TestObject), "The used memory should be equal to the size of the struct")
    }

    delete_allocator(allocator);
}

void test_overflow_allocation()
{
    PoolAllocator allocator = create_allocator(sizeof(TestObject), sizeof(TestObject), sizeof(TestObject) * 5);

    TestObject *pointers[6];

    for (int i = 0; i < 6; i++)
    {
        pointers[i] = (TestObject *)allocate(allocator);

        if (i == 6)
            ASSERT_EQ(pointers[i], nullptr, "Last allocation should fail")
        else
            ASSERT_NEQ(pointers[i], nullptr, "First allocations should not fail")
    }


    for (int i = 0; i < 6; i++)
    {
        deallocate(allocator, pointers[i]);
    }

    delete_allocator(allocator);
}

} // namespace test