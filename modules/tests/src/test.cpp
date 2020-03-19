#include "test.h"

#include <iostream>
#include <fstream>
#include <unistd.h>
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

        double mem_usage, rss;
        process_mem_usage(mem_usage, rss);

        test_functions[i]();

        double post_mem_usage, post_rss;
        process_mem_usage(post_mem_usage, post_rss);

        if (post_mem_usage != mem_usage)
        {
            test_failed = true;
        }

        if (!did_fail())
        {
            printf("✅\n");
        }
        else
        {
            fail_count++;
        }

        if (post_mem_usage != mem_usage)
        {
            printf("❌\tMEMORY LEAK DETECTED: %f \n", post_mem_usage - mem_usage);
        }
    }

    printf("\nTests finished!\n%d/%d passed\n", test_count - fail_count, test_count);
}

void process_mem_usage(double &vm_usage, double &resident_set)
{
    vm_usage = 0.0;
    resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
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
