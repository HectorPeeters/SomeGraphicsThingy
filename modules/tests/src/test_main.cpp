#include <stdio.h>

#include "magica_voxel_test.h"
#include "basic_test.h"
#include "test.h"

void (*function[])() = {
    test::test_assert,
    test::test_assert_true,
    test::test_assert_false,
    test::test_assert_eq,
    test::test_assert_neq,
    
    test::voxel_test_invalid_file
};

int main(int argc, char** argv) 
{
    test::test_start(function, sizeof(function) / sizeof(void*));
}