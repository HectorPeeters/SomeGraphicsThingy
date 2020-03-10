#include "test.h"

#include <stdio.h>

namespace test
{

bool test_failed;

void test_start(void (*test_functions[])(), unsigned int test_count)
{
    printf("Starting %d tests...\n\n", test_count);

    int fail_count = 0;

    for (int i = 0; i < test_count; i++)
    {
        test_prepare();

        printf("%d/%d ", i + 1, test_count);

        test_functions[i]();

        if (!did_fail())
            printf("✅\n");
        else
            fail_count++;
    }

    printf("\nTests finished!\n%d/%d passed\n", test_count - fail_count, test_count);
}

void test_prepare()
{
    test_failed = false;
}

bool did_fail()
{
    return test_failed;
}

void throw_error(const char *message)
{
    printf("❌\tError: %s\n", message);
    test_failed = true;
}

} // namespace test
