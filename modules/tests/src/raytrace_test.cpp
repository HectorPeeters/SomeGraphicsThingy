#include "raytrace_test.h"

#include "test.h"

#include "raytracer.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
// #include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>

namespace test
{

void test_rt_voxel_intersect()
{
    // rt::Ray ray = {
    //     origin: glm::vec3(0, 0, 0),
    //     direction: glm::vec3(0, 0, 1),
    // };

    // rt::HitData result = rt::intersect_voxel(ray, glm::vec3(0, 0, 2), 0.0f, 1000000.0f);
    
    // ASSERT_TRUE(result.hit, "Voxel intersection should return a hit");

    // result = rt::intersect_voxel(ray, glm::vec3(2, 0, 2), 0.0f, 1000000.0f);

    // ASSERT_FALSE(result.hit, "Voxel intersection should not return a hit");
}

void test_screen_space_conversion()
{
    glm::vec3 original(1.0f, -2.0f, 3.0f);

    glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -10.0f));

    glm::mat4 projection = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1000.0f);

    glm::vec4 viewport(0.0f, 0.0f, 640.0f, 360.0f);

    glm::vec3 projected = glm::project(original, model, projection, viewport);

    glm::vec3 unprojected = glm::unProject(projected, model, projection, viewport);

    float diff = glm::length(glm::abs(original - unprojected));

    ASSERT_TRUE((diff <= 0.00001f), "Original vector should equal unprojected vector");
}

} // namespace test