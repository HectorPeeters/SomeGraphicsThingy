#include <stdio.h>

#include "test.h"

#include "basic_test.h"
#include "magica_voxel_test.h"

void (*functions[])() = {
    test::test_assert,
    test::test_assert_true,
    test::test_assert_false,
    test::test_assert_eq,
    test::test_assert_neq,
    
    test::voxel_test_invalid_file,
};

int main(int argc, char** argv) 
{
    test::test_start(functions, sizeof(functions) / sizeof(void*));
}