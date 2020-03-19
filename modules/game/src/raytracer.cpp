#include "raytracer.h"

#include <cmath>
#include <stdio.h>

namespace rt
{

float *texture_data;
glm::vec4 top_left;
glm::vec4 bottom_right;
glm::vec4 horizontal;
glm::vec4 vertical;
int frame = 0;

float *raytrace_get_data()
{
    return texture_data;
}

void raytrace_resize(unsigned int width, unsigned int height)
{
    if (texture_data != nullptr)
        delete[] texture_data;

    texture_data = new float[width * height * 4];
}

void calculate_screen_bounds(glm::mat4 &projection_matrix, glm::mat4 &view_matrix)
{
    glm::mat4 inv_proj_view = glm::inverse(projection_matrix * view_matrix);

    glm::vec4 center = inv_proj_view * glm::vec4(0, 0, 0, 0);

    top_left = inv_proj_view * glm::vec4(-1, -1, 0, 0);
    bottom_right = inv_proj_view * glm::vec4(1, 1, 0, 0);

    printf("%f %f %f ; %f %f %f\n", top_left.x, top_left.y, top_left.z, bottom_right.x, bottom_right.y, bottom_right.z);
}

bool hit_sphere(const glm::vec3 &center, float radius, const glm::vec3 &origin, const glm::vec3 &dir)
{
    glm::vec3 oc = origin - center;
    float a = glm::dot(dir, dir);
    float b = 2.0 * glm::dot(oc, dir);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

void raytrace_render_scene(unsigned int width, unsigned int height, glm::mat4 &projection_matrix, glm::mat4 &view_matrix, glm::vec3 &camera_pos)
{
    if (texture_data == nullptr)
        return;

    // calculate_screen_bounds(projection_matrix, view_matrix);

    glm::mat4 inv_proj_view = glm::inverse(view_matrix * projection_matrix);

    const int subdivide_count = 2;

    frame++;
    frame %= subdivide_count * subdivide_count;

    const glm::vec4 view_port = glm::vec4(0, 0, width, height);

    glm::vec3 ray_00 = inv_proj_view * glm::vec4(-1, -1, 0.1, 0);
    glm::vec3 ray_01 = inv_proj_view * glm::vec4(-1, 1, 0.1, 0);
    glm::vec3 ray_10 = inv_proj_view * glm::vec4(1, -1, 0.1, 0);
    glm::vec3 ray_11 = inv_proj_view * glm::vec4(1, 1, 0.1, 0);

    // glm::vec3 ray_00(1, 0, 0);
    // glm::vec3 ray_01(0, 1, 0);
    // glm::vec3 ray_10(0, 0, 1);
    // glm::vec3 ray_11(1, 0, 1);

    glm::vec3 result = ray_01 - ray_00;

    for (int y = frame / subdivide_count; y < height; y += subdivide_count)
    {
        glm::vec3 left = ray_00 + (ray_01 - ray_00) * (y / (float)height);
        glm::vec3 right = ray_10 + (ray_11 - ray_10) * (y / (float)height);

        for (int x = frame % subdivide_count; x < width; x += subdivide_count)
        {
            glm::vec3 ray_dir = glm::normalize(left + (right - left) * (x / (float)width));

            if (hit_sphere(glm::vec3(0, 0, -10), 1, glm::vec3(0, 0, 0), ray_dir))
            {
                texture_data[(x + y * width) * 4 + 0] = 1.0;
                texture_data[(x + y * width) * 4 + 1] = 1.0;
                texture_data[(x + y * width) * 4 + 2] = 1.0;
                texture_data[(x + y * width) * 4 + 3] = 1.0;
            }
            else
            {
                texture_data[(x + y * width) * 4 + 0] = 0;
                texture_data[(x + y * width) * 4 + 1] = 0;
                texture_data[(x + y * width) * 4 + 2] = 0;
                texture_data[(x + y * width) * 4 + 3] = 1.0;
            }
        }
    }
}

Ray generate_ray(float u, float v)
{
}

HitData get_hit(Ray &ray, std::vector<glm::vec4> voxels)
{

    HitData closestHit;

    for (auto vox : voxels)
    {
    }

    return HitData{hit : true};
}

HitData intersect_voxel(Ray &ray, glm::vec3 voxel, float tmin, float tmax)
{
    glm::vec3 invDir = 1.0f / ray.direction;
    glm::vec3 t0 = (voxel - ray.origin) * invDir;
    glm::vec3 t1 = (voxel + glm::vec3(1, 1, 1) - ray.origin) * invDir;

    for (int a = 0; a < 3; ++a)
    {
        if (invDir[a] < 0.0f)
        {
            float temp = t1[a];
            t1[a] = t0[a];
            t0[a] = temp;
        }

        tmin = fmax(tmin, t0[a]);
        tmax = fmin(tmax, t1[a]);

        if (tmax <= tmin)
            return {hit : false};
    }

    return {hit : true};
}

} // namespace rt