#pragma once

namespace test 
{

void test_start(void (*test_functions[])(), unsigned int test_count);

void test_prepare();
bool did_fail();

void throw_error(const char* message);

#define ASSERT(x, msg) if (!x) test::throw_error(msg);
#define ASSERT_TRUE(x, msg) if (!x) test::throw_error(msg);
#define ASSERT_FALSE(x, msg) if (x) test::throw_error(msg);
#define ASSERT_EQ(x, y, msg) if (x != y) test::throw_error(msg);
#define ASSERT_NEQ(x, y, msg) if (x == y) test::throw_error(msg);

}