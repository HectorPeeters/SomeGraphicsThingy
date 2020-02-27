#include "raytracer.h"

HitData get_hit(Ray &ray, std::vector<glm::vec4> voxels)
{
    
    for (auto vox : voxels)
    {

    }
    return HitData{hit : true};
}

HitData intersect_voxel(Ray &ray, glm::vec3 voxel)
{
    glm::vec3 max = voxel + glm::vec3(1, 1, 1);

    float tmin = (voxel.x - ray.origin.x) / ray.direction.x;
    float tmax = (voxel.y - ray.origin.y) / ray.direction.y;

    if (tmin > tmax)
    {
        float temp = tmin;
        tmin = tmax;
        tmax = temp;
    }

    float tymin = (voxel.y - ray.origin.y) / ray.direction.y;
    float tymax = ((voxel.y + 1) - ray.origin.y) / ray.direction.y;

    if (tymin > tymax)
    {
        float temp = tymin;
        tymin = tymax;
        tymax = temp;
    }

    if ((tmin > tymax) || (tymin > tmax))
        return {hit : false};

    float tzmin = (voxel.z - ray.origin.z) / ray.direction.z;
    float tzmax = ((voxel.z + 1) - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax)
    {
        float temp = tzmin;
        tzmin = tzmax;
        tzmax = temp;
    }

    if ((tmin > tzmax) || (tzmin > tmax))
        return {hit : false};

    return {hit : true};
}