#include <stdio.h>

#include "test.h"

#include "magica_voxel_test.h"
#include "basic_test.h"
#include "allocator_test.h"

void (*functions[])() = {
    test::test_assert,
    test::test_assert_true,
    test::test_assert_false,
    test::test_assert_eq,
    test::test_assert_neq,
    
    test::voxel_test_invalid_file,

    test::test_simple_allocation,
    test::test_multiple_single_allocation,
    test::test_multiple_allocation,
    test::test_overflow_allocation,
};

int main(int argc, char** argv) 
{
    test::test_start(functions, sizeof(functions) / sizeof(void*));
}