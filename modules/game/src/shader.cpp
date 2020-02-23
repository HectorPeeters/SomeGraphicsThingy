#include "shader.h"

#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

Shader *bound_shader;

bool load_shader_program(const char *vertex_path, const char *fragment_path, Shader &result)
{
    std::string vertex_code;
    std::string fragment_code;
    std::ifstream vertex_stream;
    std::ifstream fragment_stream;

    vertex_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertex_stream.open(vertex_path);
        fragment_stream.open(fragment_path);
        std::stringstream vertex_string_stream, fragment_string_stream;

        vertex_string_stream << vertex_stream.rdbuf();
        fragment_string_stream << fragment_stream.rdbuf();

        vertex_stream.close();
        fragment_stream.close();

        vertex_code = vertex_string_stream.str();
        fragment_code = fragment_string_stream.str();
    }
    catch (std::ifstream::failure e)
    {
        fprintf(stderr, "Shader Error: Failed to load shader %s and %s\n", vertex_path, fragment_path);
        return false;
    }

    unsigned int vertex_id, fragment_id;
    int success;
    char info_log[512];

    const char *vertex_code_c = vertex_code.c_str();
    const char *fragment_code_c = fragment_code.c_str();

    vertex_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_id, 1, &vertex_code_c, NULL);
    glCompileShader(vertex_id);

    glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_id, 512, NULL, info_log);
        fprintf(stderr, "Shader Error: Failed to compile vertex shader:\n%s\n", info_log);
        return false;
    }

    fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_id, 1, &fragment_code_c, NULL);
    glCompileShader(fragment_id);

    glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_id, 512, NULL, info_log);
        fprintf(stderr, "Shader Error: Failed to compile fragment shader:\n%s\n", info_log);
        return false;
    }

    unsigned int program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program_id, 512, NULL, info_log);
        fprintf(stderr, "Shader Error: Failed to link shader program %s, %s: %s\n", vertex_path, fragment_path, info_log);
        return false;
    }

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    result.id = program_id;
    result.name = std::string(vertex_path);

    int uniform_count;
    glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &uniform_count);
    int attribute_count;
    glGetProgramiv(program_id, GL_ACTIVE_ATTRIBUTES, &attribute_count);

    int length, size;
    GLenum type;
    for (int i = 0; i < uniform_count; i++)
    {
        char name[64];
        glGetActiveUniform(program_id, (GLuint)i, 64, &length, &size, &type, name);

        unsigned int uniform_location = glGetUniformLocation(program_id, name);

        result.uniforms.insert(std::pair<std::string, unsigned int>(std::string(name), uniform_location));
    }

    // for (int i = 0; i < attribute_count; i++)
    // {
    //     glGetActiveAttrib(program_id, (GLuint)i, 64, &length, &size, &type, game_state.attribute_names[i]);
    // }

    printf("Loaded shader (%d) %s, %s\n", program_id, vertex_path, fragment_path);

    return true;
}

void delete_shader(Shader &shader)
{
    glUseProgram(0);
    glDeleteProgram(shader.id);
    printf("Destroyed shader %d\n", shader.id);
}

void bind_shader(Shader &shader)
{
    glUseProgram(shader.id);
    bound_shader = &shader;
}

void unbind_shader(Shader &shader)
{
    glUseProgram(0);
    bound_shader = nullptr;
}

bool uniform_1i(std::string name, int value)
{
    if (bound_shader == nullptr)
        return false;

#ifdef DEBUG_SHADER
    if (bound_shader->uniforms.find(name) == bound_shader->uniforms.end())
    {
        printf("Uniform '%s' in '%s' does not exist\n", name.c_str(), bound_shader->name.c_str());
        return false;
    }
#endif

    unsigned int uniform_location = bound_shader->uniforms[name];

    if (uniform_location == 0)
        return false;

    glUniform1i(uniform_location, value);

    return true;
}

bool uniform_1f(std::string name, float value)
{
    if (bound_shader == nullptr)
        return false;

#ifdef DEBUG_SHADER
    if (bound_shader->uniforms.find(name) == bound_shader->uniforms.end())
    {
        printf("Uniform '%s' in '%s' does not exist\n", name.c_str(), bound_shader->name.c_str());
        return false;
    }
#endif

    unsigned int uniform_location = bound_shader->uniforms[name];
    if (uniform_location == 0)
        return false;

    glUniform1f(uniform_location, value);

    return true;
}

bool uniform_3f(std::string name, glm::vec3 &value)
{
    if (bound_shader == nullptr)
        return false;

#ifdef DEBUG_SHADER
    if (bound_shader->uniforms.find(name) == bound_shader->uniforms.end())
    {
        printf("Uniform '%s' in '%s' does not exist\n", name.c_str(), bound_shader->name.c_str());
        return false;
    }
#endif

    unsigned int uniform_location = bound_shader->uniforms[name];
    if (uniform_location == 0)
        return false;

    glUniform3f(uniform_location, value.x, value.y, value.z);

    return true;
}

bool uniform_4f(std::string name, glm::vec4 &value)
{
    if (bound_shader == nullptr)
        return false;

#ifdef DEBUG_SHADER
    if (bound_shader->uniforms.find(name) == bound_shader->uniforms.end())
    {
        printf("Uniform '%s' in '%s' does not exist\n", name.c_str(), bound_shader->name.c_str());
        return false;
    }
#endif

    unsigned int uniform_location = bound_shader->uniforms[name];

    glUniform4f(uniform_location, value.x, value.y, value.z, value.w);

    return true;
}

bool uniform_mat4(std::string name, glm::mat4 &value)
{
    if (bound_shader == nullptr)
        return false;

#ifdef DEBUG_SHADER
    if (bound_shader->uniforms.find(name) == bound_shader->uniforms.end())
    {
        printf("Uniform '%s' in '%s' does not exist\n", name.c_str(), bound_shader->name.c_str());
        return false;
    }
#endif

    unsigned int uniform_location = bound_shader->uniforms[name];
    if (uniform_location == 0)
        return false;

    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));

    return true;
}