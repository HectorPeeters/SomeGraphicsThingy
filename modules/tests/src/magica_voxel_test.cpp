#include "magica_voxel_test.h"

#include "test.h"

#include "magica_voxel.h"

namespace test
{

void voxel_test_invalid_file()
{
    VoxelMesh mesh;
    ASSERT_FALSE(read_voxel("non_existing_file.vox", mesh), "read_voxel on non-existing file shoud return false");
}

}
