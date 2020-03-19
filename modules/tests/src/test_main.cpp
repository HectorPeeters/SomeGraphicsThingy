#include <stdio.h>

#include "test.h"

#include "basic_test.h"
#include "magica_voxel_test.h"
#include "raytrace_test.h"

void (*functions[])() = {
    test::test_assert,
    test::test_assert_true,
    test::test_assert_false,
    test::test_assert_eq,
    test::test_assert_neq,
    test::test_memory_leak_detection,
    test::test_glm,

    test::voxel_test_invalid_file,

    test::test_rt_voxel_intersect,
    test::test_screen_space_conversion,
};

int main(int argc, char** argv) 
{
    test::test_start(functions, sizeof(functions) / sizeof(void*));
}