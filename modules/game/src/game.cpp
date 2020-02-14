#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#define EXPORT_METHOD extern "C"

struct GameState
{
    int basic_shader;
    int uniform_count;
    int attribute_count;
    char uniform_names[64][64];
    char attribute_names[64][64];

    int fbo_shader;
    unsigned int fbo;
    unsigned int fbo_rbo;
    unsigned int fbo_texture;

    unsigned int quad_vao;
    unsigned int quad_vertices_vbo;
    unsigned int quad_texture_vbo;

    unsigned int transfom_loc;
    unsigned int projection_loc;

    unsigned int camera_pos_loc;

    unsigned int light_pos_loc;
    unsigned int light_ambient_loc;
    unsigned int light_diffuse_loc;
    unsigned int light_specular_loc;

    unsigned int material_ambient_loc;
    unsigned int material_diffuse_loc;
    unsigned int material_specular_loc;
    unsigned int material_shininess_loc;

    unsigned int triangle_mesh_position_vbo;
    unsigned int triangle_mesh_color_vbo;

    unsigned int triangle_mesh_vao;
    unsigned int triangle_mesh_ibo;

    glm::vec3 light_position;
    glm::vec3 light_ambient{0.0f, 0.5f, 0.0f};
    glm::vec3 light_diffuse{0.0f, 0.5f, 0.0f};
    glm::vec3 light_specular{0.0f, 0.5f, 0.0f};

    glm::vec3 material_ambient{0.0f, 0.5f, 0.0f};
    glm::vec3 material_diffuse{0.0f, 0.5f, 0.0f};
    glm::vec3 material_specular{0.0f, 0.5f, 0.0f};
    int material_shininess = 32;
};

struct SharedData
{
    int width;
    int height;
};

GameState game_state;
SharedData *shared_data;

int load_shader_program(const char *vertex_path, const char *fragment_path)
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
        return -1;
    }

    int vertex_id, fragment_id;
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
        return -1;
    }

    fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_id, 1, &fragment_code_c, NULL);
    glCompileShader(fragment_id);

    glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_id, 512, NULL, info_log);
        fprintf(stderr, "Shader Error: Failed to compile fragment shader:\n%s\n", info_log);
        return -1;
    }

    int program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program_id, 512, NULL, info_log);
        fprintf(stderr, "Shader Error: Failed to link shader program %s, %s\n", vertex_path, fragment_path);
        return -1;
    }

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    // glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &game_state.uniform_count);
    // glGetProgramiv(program_id, GL_ACTIVE_ATTRIBUTES, &game_state.attribute_count);

    // int length, size;
    // GLenum type;
    // for (int i = 0; i < game_state.uniform_count; i++)
    //     glGetActiveUniform(program_id, (GLuint)i, 64, &length, &size, &type, game_state.uniform_names[i]);

    // for (int i = 0; i < game_state.attribute_count; i++)
    //     glGetActiveAttrib(program_id, (GLuint)i, 64, &length, &size, &type, game_state.attribute_names[i]);

    printf("Loaded shader (%d) %s, %s\n", program_id, vertex_path, fragment_path);

    return program_id;
}

void delete_shader(int program_id)
{
    glUseProgram(0);
    glDeleteProgram(program_id);
    printf("Destroyed shader %d\n", program_id);
}

void load_triangle_mesh()
{
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };

    float colors[] = {
        0.0f, 1.0f, 1.0f, // top right
        1.0f, 0.0f, 0.0f, // bottom right
        0.0f, 1.0f, 0.0f, // bottom left
        0.0f, 0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    glGenVertexArrays(1, &game_state.triangle_mesh_vao);
    glGenBuffers(1, &game_state.triangle_mesh_position_vbo);
    glGenBuffers(1, &game_state.triangle_mesh_color_vbo);
    glGenBuffers(1, &game_state.triangle_mesh_ibo);

    glBindVertexArray(game_state.triangle_mesh_vao);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.triangle_mesh_position_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.triangle_mesh_color_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, game_state.triangle_mesh_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void delete_triangle_mesh()
{
    glDeleteVertexArrays(1, &game_state.triangle_mesh_vao);
    glDeleteBuffers(1, &game_state.triangle_mesh_position_vbo);
    glDeleteBuffers(1, &game_state.triangle_mesh_color_vbo);
    glDeleteBuffers(1, &game_state.triangle_mesh_ibo);
}

void load_fbo_quad()
{
    float vertices[] = {
        -1.0f,
        1.0f,
        -1.0f,
        -1.0f,
        1.0f,
        -1.0f,

        -1.0f,
        1.0f,
        1.0f,
        -1.0f,
        1.0f,
        1.0f,
    };

    float texture_coords[] = {
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        1.0f,
    };

    glGenVertexArrays(1, &game_state.quad_vao);
    glGenBuffers(1, &game_state.quad_vertices_vbo);
    glGenBuffers(1, &game_state.quad_texture_vbo);

    glBindVertexArray(game_state.quad_vao);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.quad_vertices_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.quad_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coords), texture_coords, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void delete_fbo_quad()
{
    glDeleteVertexArrays(1, &game_state.quad_vao);
    glDeleteBuffers(1, &game_state.quad_vertices_vbo);
    glDeleteBuffers(1, &game_state.quad_texture_vbo);
}

unsigned int generate_texture(unsigned int width, unsigned int height)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return id;
}

unsigned int generate_render_buffer(unsigned int width, unsigned int height)
{
    unsigned int id;

    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    return id;
}

unsigned int generate_fbo(unsigned int texture_id, unsigned int render_buffer_id)
{
    unsigned int id;
    glGenFramebuffers(1, &id);

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    if (texture_id != -1)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
    }

    if (render_buffer_id != -1)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_id);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Failed to build framebuffer\n");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &id);
        return -1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return id;
}

EXPORT_METHOD bool init(void *shared_data_location)
{
    // glEnable(GL_FRAMEBUFFER_SRGB); 

    shared_data = (SharedData *)shared_data_location;

    game_state.basic_shader = load_shader_program("res/shaders/basic.vert", "res/shaders/basic.frag");
    if (game_state.basic_shader == -1)
        return false;

    game_state.fbo_shader = load_shader_program("res/shaders/fbo.vert", "res/shaders/fbo.frag");
    if (game_state.fbo_shader == -1)
        return false;

    game_state.fbo_texture = generate_texture(shared_data->width, shared_data->height);
    game_state.fbo_rbo = generate_render_buffer(shared_data->width, shared_data->height);
    game_state.fbo = generate_fbo(game_state.fbo_texture, game_state.fbo_rbo);
    load_fbo_quad();

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0, 0, -2));
    trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(1.0, 1.0, 1.0));

    game_state.transfom_loc = glGetUniformLocation(game_state.basic_shader, "u_transform");
    glUseProgram(game_state.basic_shader);
    glUniformMatrix4fv(game_state.transfom_loc, 1, GL_FALSE, glm::value_ptr(trans));

    game_state.projection_loc = glGetUniformLocation(game_state.basic_shader, "u_projection");
    game_state.camera_pos_loc = glGetUniformLocation(game_state.basic_shader, "u_cameraPos");

    game_state.light_pos_loc = glGetUniformLocation(game_state.basic_shader, "u_light.position");
    game_state.light_ambient_loc = glGetUniformLocation(game_state.basic_shader, "u_light.ambient");
    game_state.light_diffuse_loc = glGetUniformLocation(game_state.basic_shader, "u_light.diffuse");
    game_state.light_specular_loc = glGetUniformLocation(game_state.basic_shader, "u_light.specular");

    game_state.material_ambient_loc = glGetUniformLocation(game_state.basic_shader, "u_material.ambient");
    game_state.material_diffuse_loc = glGetUniformLocation(game_state.basic_shader, "u_material.diffuse");
    game_state.material_specular_loc = glGetUniformLocation(game_state.basic_shader, "u_material.specular");
    game_state.material_shininess_loc = glGetUniformLocation(game_state.basic_shader, "u_material.shininess");

    game_state.light_position.z = -2.07f;

    load_triangle_mesh();

    return true;
}

EXPORT_METHOD void deinit()
{
    glDeleteRenderbuffers(1, &game_state.fbo_rbo);
    glDeleteFramebuffers(1, &game_state.fbo);

    delete_triangle_mesh();

    delete_shader(game_state.basic_shader);
}

EXPORT_METHOD void imgui_draw()
{
    ImGui::Begin("Shader Viewer");

    ImGui::Text("Id: %d", game_state.basic_shader);
    ImGui::Text("Uniforms: %d", game_state.uniform_count);
    ImGui::Text("Attributes: %d", game_state.attribute_count);

    if (ImGui::CollapsingHeader("Uniforms"))
    {
        for (int i = 0; i < game_state.uniform_count; i++)
            ImGui::Text("%s\n", game_state.uniform_names[i]);
    }

    if (ImGui::CollapsingHeader("Attributes"))
    {
        for (int i = 0; i < game_state.attribute_count; i++)
            ImGui::Text("%s\n", game_state.attribute_names[i]);
    }

    ImGui::End();

    ImGui::Begin("Rendering");

    if (ImGui::CollapsingHeader("Lighting"))
    {
        ImGui::SliderFloat("X", &game_state.light_position.x, -1, 1);
        ImGui::SliderFloat("Y", &game_state.light_position.y, -1, 1);
        ImGui::SliderFloat("Z", &game_state.light_position.z, -2.0, -2.5);

        ImGui::SliderFloat3("Light Ambient", glm::value_ptr(game_state.light_ambient), 0, 1);
        ImGui::SliderFloat3("Light Diffuse", glm::value_ptr(game_state.light_diffuse), 0, 1);
        ImGui::SliderFloat3("Light Specular", glm::value_ptr(game_state.light_specular), 0, 1);
    }

    if (ImGui::CollapsingHeader("Material"))
    {
        ImGui::SliderFloat3("Mat Ambient", glm::value_ptr(game_state.material_ambient), 0, 1);
        ImGui::SliderFloat3("Mat Diffuse", glm::value_ptr(game_state.material_diffuse), 0, 1);
        ImGui::SliderFloat3("Mat Specular", glm::value_ptr(game_state.material_specular), 0, 1);
        ImGui::SliderInt("Light Shininess", &game_state.material_shininess, 0, 100);
    }

    ImGui::End();
}

EXPORT_METHOD void resize(unsigned int width, unsigned int height)
{
    glDeleteTextures(1, &game_state.fbo_texture);
    glDeleteRenderbuffers(1, &game_state.fbo_rbo);
    glDeleteFramebuffers(1, &game_state.fbo);

    game_state.fbo_texture = generate_texture(width, height);
    game_state.fbo_rbo = generate_render_buffer(width, height);
    game_state.fbo = generate_fbo(game_state.fbo_texture, game_state.fbo_rbo);
}

float i = 0;
EXPORT_METHOD void update(float delta)
{
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)shared_data->width / (float)shared_data->height, 0.1f, 1000.0f);
    glUseProgram(game_state.basic_shader);
    glUniformMatrix4fv(game_state.projection_loc, 1, GL_FALSE, glm::value_ptr(proj));

    game_state.light_position.y = sin(i) / 3.0f;
    game_state.light_position.x = cos(i) / 3.0f;
    i += delta;

    glUseProgram(game_state.basic_shader);
    glUniform3f(game_state.light_pos_loc, game_state.light_position.x, game_state.light_position.y, game_state.light_position.z);
    glUniform3f(game_state.light_ambient_loc, game_state.light_ambient.x, game_state.light_ambient.y, game_state.light_ambient.z);
    glUniform3f(game_state.light_diffuse_loc, game_state.light_diffuse.x, game_state.light_diffuse.y, game_state.light_diffuse.z);
    glUniform3f(game_state.light_specular_loc, game_state.light_specular.x, game_state.light_specular.y, game_state.light_specular.z);

    glUniform3f(game_state.material_ambient_loc, game_state.material_ambient.x, game_state.material_ambient.y, game_state.material_ambient.z);
    glUniform3f(game_state.material_diffuse_loc, game_state.material_diffuse.x, game_state.material_diffuse.y, game_state.material_diffuse.z);
    glUniform3f(game_state.material_specular_loc, game_state.material_specular.x, game_state.material_specular.y, game_state.material_specular.z);
    glUniform1i(game_state.material_shininess_loc, game_state.material_shininess);

    glUniform3f(game_state.camera_pos_loc, 0, 0, 0);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0, 0, -2));
    trans = glm::rotate(trans, glm::radians(float(sin(i / 2.0) * 5.0)), glm::vec3(0.0, 1.0, 0.0));
    trans = glm::scale(trans, glm::vec3(1.0, 1.0, 1.0));

    glUniformMatrix4fv(game_state.transfom_loc, 1, GL_FALSE, glm::value_ptr(trans));
}

EXPORT_METHOD void render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, game_state.fbo);
    {
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(game_state.basic_shader);
        glBindVertexArray(game_state.triangle_mesh_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    {
        glUseProgram(game_state.fbo_shader);
        glBindVertexArray(game_state.quad_vao);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, game_state.fbo_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
