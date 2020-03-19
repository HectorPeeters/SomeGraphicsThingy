#include "basic_test.h"

#include "test.h"
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>

namespace test
{

void test_assert()
{
    ASSERT(true, "ASSERT should not fail on true condition")
}

void test_assert_true()
{
    ASSERT_TRUE(true, "ASSERT_TRUE should not fail on true condition")
}

void test_assert_false()
{
    ASSERT_FALSE(false, "ASSERT_FALSE should not fail on false condition")
}

void test_assert_eq()
{
    int a = 5;
    int b = 5;
    bool c = false;

    ASSERT_EQ(a, a, "ASSERT_MEM_EQ should not fail on the same int object")
    ASSERT_EQ(a, b, "ASSERT_MEM_EQ should not fail on two ints with the same value")
    ASSERT_EQ(c, c, "ASSERT_MEM_EQ should not fail on the same bool object")
}

void test_assert_neq()
{
    int a = 5;
    int b = 6;
    int d = 7;

    ASSERT_NEQ(a, b, "ASSERT_MEM_NEQ should not fail on different int objects with different values")
    ASSERT_NEQ(b, d, "ASSERT_MEM_NEQ should not fail on different int objects of different types")
}

void test_memory_leak_detection()
{
    for (int i = 0; i < 1000; i++)
    {
        void* data = malloc(1000);
        free(data);
    }
}

void test_glm()
{
    glm::vec3 vec1(1, 0, 0);
    glm::vec3 vec2(0, 1, 0);

    glm::vec3 result = vec2 - vec1;

    ASSERT_EQ(result.x, -1, "Values should be equal")
    ASSERT_EQ(result.y, 1, "Values should be equal")
    ASSERT_EQ(result.z, 0, "Values should be equal")
}

}