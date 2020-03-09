#include "basic_test.h"

#include "test.h"
#include <stdio.h>

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
    bool b = false;

    ASSERT_EQ(a, a, "ASSERT_MEM_EQ should not fail on the same int object")
    ASSERT_EQ(b, b, "ASSERT_MEM_EQ should not fail on the same bool object")
}

void test_assert_neq()
{
    int a = 5;
    int b = 6;
    int d = 7;

    ASSERT_NEQ(a, b, "ASSERT_MEM_NEQ should not fail on different int objects with different values")
    ASSERT_NEQ(b, d, "ASSERT_MEM_NEQ should not fail on different int objects of different types")
}

}