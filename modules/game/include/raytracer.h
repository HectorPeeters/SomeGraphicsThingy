#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core.h"

namespace rt
{

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

struct RTTexture
{
    unsigned int texture_id;
    unsigned int width, height;
    uint8 texture_data[];
};

float* raytrace_get_data();

void calculate_screen_bounds(glm::mat4& projection_matrix, glm::mat4& view_matrix);

void raytrace_render_scene(unsigned int width, unsigned int height, glm::mat4& projection_matrix, glm::mat4& view_matrix, glm::vec3& camera_pos);

void raytrace_resize(unsigned int width, unsigned int height);

Ray generate_ray(float u, float v);

HitData get_hit(Ray &ray, std::vector<glm::vec4> voxels);
HitData intersect_voxel(Ray &ray, glm::vec3 voxel, float tmin, float tmax);

}