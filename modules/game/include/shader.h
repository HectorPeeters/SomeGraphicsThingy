#pragma once

#include <map>
#include <string>

#include <glm/glm.hpp>

struct Shader
{
    std::string name;
    unsigned int id;
    std::map<std::string, unsigned int> uniforms;
};

bool load_shader_program(const char *vertex_path, const char *fragment_path, Shader& result);

void delete_shader(Shader& shader);

void bind_shader(Shader& shader);
void unbind_shader(Shader& shader);

bool uniform_1i(std::string name, int value);
bool uniform_1f(std::string name, float value);
bool uniform_3f(std::string name, glm::vec3& value);
bool uniform_4f(std::string name, glm::vec4& value);
bool uniform_mat4(std::string name, glm::mat4& value);
