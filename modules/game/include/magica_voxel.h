#pragma once

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>

struct Voxel
{
    unsigned char z;
    unsigned char x;
    unsigned char y;
    unsigned char color_index;
};

struct VoxelData
{
    unsigned int size_x, size_y, size_z;
    unsigned int voxel_count = -1;
    unsigned int vao_id;
    unsigned int vbo_id;
    Voxel *voxels;
    unsigned int x;
    unsigned int y;
    unsigned int z;
};

struct VoxelMesh
{
    std::vector<VoxelData> sub_meshes;
    VoxelData *current_mesh = nullptr;
    glm::vec4 colors[256];
    int vao_id;
};

bool read_voxel_header(FILE *file);
bool read_voxel_chunk_size(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk_xyzi(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk_rgba(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk_string(FILE *file, long &pos);
bool read_voxel_chunk_dict(FILE *file, long &pos);
bool read_voxel_chunk_ntrn(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk_ngrp(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk_nshp(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk_layr(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk_matl(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk_robj(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel_chunk(FILE *file, VoxelMesh &result, long &pos);
bool read_voxel(const char *path, VoxelMesh &result);
void delete_voxel(VoxelMesh &data);

int switch_endian(int x);