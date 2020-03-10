#include "magica_voxel.h"

bool first_chunk = true;

// #define VOXEL_DEBUG

int voxel_count;

bool read_voxel_header(FILE *file)
{
    const unsigned char header[] = {0x56, 0x4F, 0x58, 0x20};
    unsigned char buff[4];

    if (!fread(buff, 4, 1, file))
        return false;

    bool success = memcmp(buff, header, 4) == 0;

    if (!fread(buff, 4, 1, file))
        return false;

    return success;
}

bool read_voxel_chunk_size(FILE *file, VoxelMesh &result, long &pos)
{
    if (!first_chunk)
    {
        result.sub_meshes.push_back(*result.current_mesh);
        result.current_mesh = new VoxelData();
    }
    first_chunk = false;

    if (!fread(&result.current_mesh->size_x, 4, 1, file))
        return false;
    pos += 4;
    if (!fread(&result.current_mesh->size_y, 4, 1, file))
        return false;
    pos += 4;
    if (!fread(&result.current_mesh->size_z, 4, 1, file))
        return false;
    pos += 4;

#ifdef VOXEL_DEBUG
    printf("\t%d %d %d\n", result.current_mesh->size_x, result.current_mesh->size_y, result.current_mesh->size_z);
#endif

    return true;
}

bool read_voxel_chunk_xyzi(FILE *file, VoxelMesh &result, long &pos)
{
    if (!fread(&result.current_mesh->voxel_count, 4, 1, file))
        return false;
    pos += 4;

#ifdef VOXEL_DEBUG
    printf("\t%d\n", result.current_mesh->voxel_count);
#endif

    result.current_mesh->voxels = new Voxel[result.current_mesh->voxel_count];

    for (int i = 0; i < result.current_mesh->voxel_count; i++)
    {
        if (!fread(&result.current_mesh->voxels[i], 4, 1, file))
            return false;
        pos += 4;
        voxel_count++;

        result.current_mesh->voxels[i].color_index--;

#ifdef VOXEL_DEBUG
        // printf("\t%d %d %d: %d\n", result.current_mesh->voxels[i].x, result.current_mesh->voxels[i].y, result.current_mesh->voxels[i].z, result.current_mesh->voxels[i].color_index);
#endif
    }

    return true;
}

int switch_endian(int x)
{
    return (((x >> 24) & 0x000000ff) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000) | ((x << 24) & 0xff000000));
}

bool read_voxel_chunk_rgba(FILE *file, VoxelMesh &result, long &pos)
{
    for (int i = 0; i < 256; i++)
    {

        unsigned int color;
        if (!fread(&color, 4, 1, file))
            return false;
        pos += 4;

        color = switch_endian(color);

        result.colors[i].r = ((color >> 8 * 3) & 0xFF) / 256.0f;
        result.colors[i].g = ((color >> 8 * 2) & 0xFF) / 256.0f;
        result.colors[i].b = ((color >> 8) & 0xFF) / 256.0f;
        result.colors[i].a = ((color)&0xFF) / 256.0f;

#ifdef VOXEL_DEBUG
        if (result.colors[i].r != result.colors[i].g)
        {
            printf("\t%x-> ", color);
            printf("\t%f %f %f %f\n", result.colors[i].r, result.colors[i].g, result.colors[i].b, result.colors[i].a);
        }
#endif
    }

    return true;
}

bool read_voxel_chunk_string(FILE *file, long &pos)
{
    int length;
    if (!fread(&length, 4, 1, file))
        return false;
    pos += 4;

    char *r = new char[length + 1];
    r[length] = '\0';

    for (int i = 0; i < length; i++)
    {
        if (!fread((void *)&r[i], 1, 1, file))
            return false;
        pos++;
    }

#ifdef VOXEL_DEBUG
    printf("\tString: %s\n", r);
#endif

    delete[] r;
}

bool read_voxel_chunk_dict(FILE *file, long &pos)
{
    unsigned int n;
    if (!fread(&n, 4, 1, file))
        return false;
    pos += 4;

#ifdef VOXEL_DEBUG
    printf("Dict length: %d\n", n);
#endif

    for (int i = 0; i < n; i++)
    {
        read_voxel_chunk_string(file, pos);
        read_voxel_chunk_string(file, pos);
    }

    return true;
}

bool read_voxel_chunk_ntrn(FILE *file, VoxelMesh &result, long &pos)
{
    int node_id;
    if (!fread(&node_id, 4, 1, file))
        return false;
    pos += 4;

    if (!read_voxel_chunk_dict(file, pos))
        return false;

    int child_id;
    if (!fread(&child_id, 4, 1, file))
        return false;
    pos += 4;

    int reserved_id;
    if (!fread(&reserved_id, 4, 1, file))
        return false;
    pos += 4;

    if (reserved_id != -1)
        return false;

    int layer_id;
    if (!fread(&layer_id, 4, 1, file))
        return false;
    pos += 4;

    int num_frames;
    if (!fread(&num_frames, 4, 1, file))
        return false;
    pos += 4;

    if (num_frames != 1)
        return false;

    read_voxel_chunk_dict(file, pos);

    // int dict_size;
    // if (!fread(&dict_size, 4, 1, file))
    //     return false;
    // pos += 4;

    // for (int i = 0; i < dict_size; i++)
    // {

    //     int name_size;
    //     if (!fread(&name_size, 4, 1, file))
    //         return false;
    //     pos += 4;

    //     char *name_buff = new char[name_size + 1];

    //     if (!fread(&name_buff[0], name_size, 1, file))
    //         return false;
    //     pos += name_size;

    //     name_buff[name_size] = '\0';

    //     unsigned int value_size;
    //     if (!fread(&value_size, 4, 1, file))
    //         return false;

    //     char *value = new char[value_size + 1];
    //     value[value_size] = '\0';

    //     if (!fread(value, value_size, 1, file))
    //         return false;
    //     pos += value_size;

    //     // fseek(file, value_size, SEEK_CUR);
    //     if (strcmp(name_buff, "_t") == 0)
    //     {
    //         // sscanf(value, "%i %i %i\n", &result.sub_meshes[layer_id].x, &result.sub_meshes[layer_id].y, &result.sub_meshes[layer_id].z);
    //         // printf("%i %i %i\n", result.current_mesh->x, result.current_mesh->y, result.current_mesh->z);
    //     }
    //     else if (strcmp(name_buff, "_r") == 0)
    //     {

    //     }
    //     else
    //     {
    //         fprintf(stderr, "Invalid dict entry in nTRN chunk\n");
    //         delete[] name_buff;
    //         delete[] value;
    //         return false;
    //     }

    //     delete[] name_buff;
    //     delete[] value;
    // }

    return true;
}

bool read_voxel_chunk_ngrp(FILE *file, VoxelMesh &result, long &pos)
{
    int node_id;
    if (!fread(&node_id, 4, 1, file))
        return false;
    pos += 4;

    read_voxel_chunk_dict(file, pos);

    int n;
    if (!fread(&n, 4, 1, file))
        return false;
    pos += 4;

    for (int i = 0; i < n; i++)
    {
        int child_id;
        if (!fread(&child_id, 4, 1, file))
            return false;
        pos += 4;
    }

    return true;
}

bool read_voxel_chunk_nshp(FILE *file, VoxelMesh &result, long &pos)
{
    int node_id;
    if (!fread(&node_id, 4, 1, file))
        return false;
    pos += 4;

    read_voxel_chunk_dict(file, pos);

    int num_models;
    if (!fread(&num_models, 4, 1, file))
        return false;
    pos += 4;

    if (num_models != 1)
        return false;

    for (int i = 0; i < num_models; i++)
    {
        int child_id;
        if (!fread(&child_id, 4, 1, file))
            return false;
        pos += 4;

        read_voxel_chunk_dict(file, pos);
    }

    return true;
}

bool read_voxel_chunk_layr(FILE *file, VoxelMesh &result, long &pos)
{
    int layer_id;
    if (!fread(&layer_id, 4, 1, file))
        return false;
    pos += 4;

    read_voxel_chunk_dict(file, pos);

    int reserved_id;
    if (!fread(&reserved_id, 4, 1, file))
        return false;
    pos += 4;

    if (reserved_id != -1)
        return false;

    return true;
}

bool read_voxel_chunk_matl(FILE *file, VoxelMesh &result, long &pos)
{
    int mat_id;
    if (!fread(&mat_id, 4, 1, file))
        return false;
    pos += 4;

    read_voxel_chunk_dict(file, pos);

    return true;
}

bool read_voxel_chunk_robj(FILE *file, VoxelMesh &result, long &pos)
{
    read_voxel_chunk_dict(file, pos);

    return true;
}

bool read_voxel_chunk(FILE *file, VoxelMesh &result, long &pos)
{
    char name_buff[4];
    if (!fread(name_buff, 4, 1, file))
        return false;
    pos += 4;

    int size;
    if (!fread(&size, 4, 1, file))
        return false;
    pos += 4;

    int children_size;
    if (!fread(&children_size, 4, 1, file))
        return false;
    pos += 4;

#ifdef VOXEL_DEBUG
    printf("Chunk %s: %d %d\n", name_buff, size, children_size);
#endif

    if (strcmp(name_buff, "MAIN") == 0)
    {
        return true;
    }
    else if (strcmp(name_buff, "SIZE") == 0)
    {
        return read_voxel_chunk_size(file, result, pos);
    }
    else if (strcmp(name_buff, "XYZI") == 0)
    {
        return read_voxel_chunk_xyzi(file, result, pos);
    }
    else if (strcmp(name_buff, "RGBA") == 0)
    {
        return read_voxel_chunk_rgba(file, result, pos);
    }
    else if (strcmp(name_buff, "nTRN") == 0)
    {
        return read_voxel_chunk_ntrn(file, result, pos);
    }
    else if (strcmp(name_buff, "nGRP") == 0)
    {
        return read_voxel_chunk_ngrp(file, result, pos);
    }
    else if (strcmp(name_buff, "nSHP") == 0)
    {
        return read_voxel_chunk_nshp(file, result, pos);
    }
    else if (strcmp(name_buff, "LAYR") == 0)
    {
        return read_voxel_chunk_layr(file, result, pos);
    }
    else if (strcmp(name_buff, "MATL") == 0)
    {
        return read_voxel_chunk_matl(file, result, pos);
    }
    else if (strcmp(name_buff, "rOBJ") == 0)
    {
        return read_voxel_chunk_robj(file, result, pos);
    }

    return false;
}

bool read_voxel(const char *path, VoxelMesh &result)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file: %s\n", path);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long file_length = ftell(file);
    rewind(file);

    long pos = 8;

    if (!read_voxel_header(file))
    {
        fprintf(stderr, "Failed to load voxel file '%s': voxel header not present\n", path);
        goto read_voxel_end;
    }

    result.current_mesh = new VoxelData();

    first_chunk = true;
    while (pos < file_length)
    {
        if (!read_voxel_chunk(file, result, pos))
        {
            fprintf(stderr, "Failed to load voxel file '%s': invalid chunk\n", path);
            goto read_voxel_end;
        }
    }

    if (result.current_mesh != nullptr)
    {
        result.sub_meshes.push_back(*result.current_mesh);
        result.current_mesh = nullptr;
    }

    printf("Read voxel file '%s'\n", path);
    printf("%d\n", voxel_count);

read_voxel_end:
    fclose(file);

    return true;
}

void delete_voxel(VoxelMesh &data)
{
    for (int i = 0; i < data.sub_meshes.size(); i++)
    {
        delete[] data.sub_meshes[i].voxels;
    }

    data.sub_meshes.clear();
}