#include "raytracer.h"

#include <cmath>

namespace rt
{

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