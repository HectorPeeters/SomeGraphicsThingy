#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct HitData
{
    bool hit;
    glm::vec3 hit_pos;
    glm::vec3 hit_normal;
    float distance;
};

HitData get_hit(Ray &ray, std::vector<glm::vec4> voxels);

HitData intersect_voxel(Ray &ray, glm::vec3 voxel);