#include "squirrel_pch.h"

#include <glad/glad.h>
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "magica_voxel.h"
#include "shader.h"
#include "keys.h"
#include "raytracer.h"

#define PI 3.141592653689

#define EXPORT_METHOD extern "C"

struct GameState
{
    Shader basic_shader;

    Shader fbo_shader;
    unsigned int fbo;
    unsigned int fbo_rbo;
    unsigned int fbo_texture;

    unsigned int quad_vao;
    unsigned int quad_vertices_vbo;
    unsigned int quad_texture_vbo;

    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;

    float exposure = 2.0;
    float gamma = 0.6;

    glm::vec3 camera_pos{0.0f, 0.0f, 3.0f};
    glm::vec3 camera_front{0.0f, 0.0f, -1.0f};
    glm::vec3 camera_up{0.0f, 1.0f, 0.0f};

    double last_mouse_x = -1;
    double last_mouse_y = -1;

    float pitch = 0;
    float yaw = -90;

    VoxelMesh voxel;

    Shader voxel_shader;

    unsigned int cube_vao;
    unsigned int cube_vertices_vbo;
    unsigned int cube_ibo;

    bool raytracing = true;
};

struct SharedData
{
    unsigned int width;
    unsigned int height;

    double mouse_x;
    double mouse_y;

    bool keys[1024];

    bool game_focussed = true;
};

GameState game_state;
SharedData *shared_data;

unsigned int load_texture(const char *path)
{
    int width, height, num_channels;
    unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);

    if (!data)
    {
        fprintf(stderr, "Failed to load texture: %s\n", path);
        return -1;
    }

    unsigned int texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    printf("Loaded texture (%d) %s\n", texture, path);

    return texture;
}

void load_cube_mesh()
{
    // float vertices[] = {
    //     -1.0, -1.0, 1.0,  //
    //     1.0, -1.0, 1.0,   //
    //     1.0, 1.0, 1.0,    //
    //     -1.0, 1.0, 1.0,   //
    //     -1.0, -1.0, -1.0, //
    //     1.0, -1.0, -1.0,  //
    //     1.0, 1.0, -1.0,   //
    //     -1.0, 1.0, -1.0,  //
    // };

    // unsigned int indices[] = {
    //     0, 1, 2, 2, 3, 0, //
    //     1, 5, 6, 6, 2, 1, //
    //     7, 6, 5, 5, 4, 7, //
    //     4, 0, 3, 3, 7, 4, //
    //     4, 5, 1, 1, 0, 4, //
    //     3, 2, 6, 6, 7, 3, //
    // };

    float vertices[] = {
        0.0f,
        0.0f,
        0.0f,
    };

    // unsigned int indices[] = {
    //     0,
    // };

    glGenVertexArrays(1, &game_state.cube_vao);
    glGenBuffers(1, &game_state.cube_vertices_vbo);
    // glGenBuffers(1, &game_state.cube_ibo);

    glBindVertexArray(game_state.cube_vao);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.cube_vertices_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, game_state.cube_ibo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void delete_cube_mesh()
{
    glDeleteVertexArrays(1, &game_state.cube_vao);
    glDeleteBuffers(1, &game_state.cube_vertices_vbo);
}

void load_fbo_quad()
{
    float vertices[] = {
        -1.0f, 1.0f, -1.0f, //
        -1.0f, 1.0f, -1.0f, //

        -1.0f, 1.0f, 1.0f, //
        -1.0f, 1.0f, 1.0f, //
    };

    float texture_coords[] = {
        0.0f, 1.0f, //
        0.0f, 0.0f, //
        1.0f, 0.0f, //
        0.0f, 1.0f, //
        1.0f, 0.0f, //
        1.0f, 1.0f, //
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

void convert_voxel_to_mesh(VoxelMesh &voxel)
{
    for (int i = 0; i < voxel.sub_meshes.size(); i++)
    {
        glGenVertexArrays(1, &voxel.sub_meshes[i].vao_id);
        glGenBuffers(1, &voxel.sub_meshes[i].vbo_id);

        glBindVertexArray(voxel.sub_meshes[i].vao_id);

        float *vertices = new float[voxel.sub_meshes[i].voxel_count * 4];

        for (int j = 0; j < voxel.sub_meshes[i].voxel_count; j++)
        {
            vertices[j * 4 + 0] = voxel.sub_meshes[i].voxels[j].x;
            vertices[j * 4 + 1] = voxel.sub_meshes[i].voxels[j].y;
            vertices[j * 4 + 2] = voxel.sub_meshes[i].voxels[j].z;
            vertices[j * 4 + 3] = voxel.sub_meshes[i].voxels[j].color_index;
        }

        glBindBuffer(GL_ARRAY_BUFFER, voxel.sub_meshes[i].vbo_id);
        glBufferData(GL_ARRAY_BUFFER, voxel.sub_meshes[i].voxel_count * 4 * sizeof(float), vertices, GL_STATIC_DRAW);

        delete[] vertices;

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

void update_camera()
{
    const float camera_speed = 0.05f;
    const float mouse_sensitivity = 0.05f;

    if (game_state.last_mouse_x == -1 || game_state.last_mouse_x == -1)
    {
        game_state.last_mouse_x = shared_data->mouse_x;
        game_state.last_mouse_y = shared_data->mouse_y;
    }

    double dx = shared_data->mouse_x - game_state.last_mouse_x;
    double dy = game_state.last_mouse_y - shared_data->mouse_y;

    game_state.last_mouse_x = shared_data->mouse_x;
    game_state.last_mouse_y = shared_data->mouse_y;

    if (shared_data->game_focussed)
    {

        if (shared_data->keys[KEY_W])
            game_state.camera_pos += camera_speed * game_state.camera_front;
        if (shared_data->keys[KEY_S])
            game_state.camera_pos -= camera_speed * game_state.camera_front;

        if (shared_data->keys[KEY_A])
            game_state.camera_pos -= glm::normalize(glm::cross(game_state.camera_front, game_state.camera_up)) * camera_speed;
        if (shared_data->keys[KEY_D])
            game_state.camera_pos += glm::normalize(glm::cross(game_state.camera_front, game_state.camera_up)) * camera_speed;

        if (shared_data->keys[KEY_SPACE])
            game_state.camera_pos.y += camera_speed;
        if (shared_data->keys[KEY_LEFT_SHIFT])
            game_state.camera_pos.y -= camera_speed;

        dx *= mouse_sensitivity;
        dy *= mouse_sensitivity;

        game_state.yaw += dx;
        game_state.pitch += dy;

        if (game_state.pitch > 89.0f)
            game_state.pitch = 89.0f;
        if (game_state.pitch < -89.0f)
            game_state.pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(game_state.yaw)) * cos(glm::radians(game_state.pitch));
        direction.y = sin(glm::radians(game_state.pitch));
        direction.z = sin(glm::radians(game_state.yaw)) * cos(glm::radians(game_state.pitch));
        game_state.camera_front = glm::normalize(direction);

        game_state.view_matrix = glm::lookAt(game_state.camera_pos, game_state.camera_pos + game_state.camera_front, game_state.camera_up);
    }
}

EXPORT_METHOD bool init(void *shared_data_location)
{
    shared_data = (SharedData *)shared_data_location;

    if (!load_shader_program("res/shaders/fbo.vert", "res/shaders/fbo.frag", game_state.fbo_shader))
        return false;

    if (!load_shader_program("res/shaders/voxel.vert", "res/shaders/voxel.frag", game_state.voxel_shader))
        return false;

    game_state.fbo_texture = generate_texture(shared_data->width, shared_data->height);
    game_state.fbo_rbo = generate_render_buffer(shared_data->width, shared_data->height);
    game_state.fbo = generate_fbo(game_state.fbo_texture, game_state.fbo_rbo);

    load_fbo_quad();
    load_cube_mesh();

    if (!read_voxel("res/models/monu9.vox", game_state.voxel))
        fprintf(stderr, "Failed to open file");
    convert_voxel_to_mesh(game_state.voxel);

    bind_shader(game_state.voxel_shader);
    glUniform4fv(glGetUniformLocation(game_state.voxel_shader.id, "u_colors"), 256, glm::value_ptr(game_state.voxel.colors[0]));

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_PROGRAM_POINT_SIZE);

    return true;
}

EXPORT_METHOD void deinit()
{
    glDeleteRenderbuffers(1, &game_state.fbo_rbo);
    glDeleteFramebuffers(1, &game_state.fbo);

    delete_fbo_quad();

    delete_shader(game_state.fbo_shader);
    delete_shader(game_state.voxel_shader);

    delete_cube_mesh();

    delete_voxel(game_state.voxel);
}

EXPORT_METHOD void imgui_draw()
{
    ImGui::Begin("Shader Viewer");

    ImGui::Text("Id: %d", game_state.voxel_shader.id);
    ImGui::Text("Uniforms: %lu", game_state.voxel_shader.uniforms.size());

    if (ImGui::CollapsingHeader("Uniforms"))
    {
        for (auto pair : game_state.voxel_shader.uniforms)
        {
            ImGui::Text("%s: %d", pair.first.c_str(), pair.second);
        }
    }

    ImGui::End();

    ImGui::Begin("Rendering");

    ImGui::Checkbox("Raytracing", &game_state.raytracing);
    ImGui::Spacing();
    ImGui::SliderFloat("Exposure", &game_state.exposure, 0, 5);
    ImGui::SliderFloat("Gamma", &game_state.gamma, 0, 5);

    ImGui::End();

    ImGui::Begin("Input");
    ImGui::Text("Mouse X: %f", shared_data->mouse_x);
    ImGui::Text("Mouse Y: %f", shared_data->mouse_y);
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

    rt::raytrace_resize(width, height);
}

EXPORT_METHOD void update(float delta)
{
    game_state.projection_matrix = glm::perspective(glm::radians(45.0f), (float)shared_data->width / (float)shared_data->height, 0.1f, 1000.0f);

    update_camera();

    bind_shader(game_state.fbo_shader);
    uniform_1f("u_exposure", game_state.exposure);
    uniform_1f("u_gamma", game_state.gamma);

    bind_shader(game_state.voxel_shader);
    uniform_mat4("u_view", game_state.view_matrix);
    uniform_mat4("u_projection", game_state.projection_matrix);
}

EXPORT_METHOD void render()
{
    if (game_state.raytracing)
    {
        rt::raytrace_render_scene(shared_data->width, shared_data->height, game_state.projection_matrix, game_state.view_matrix, game_state.camera_pos);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, shared_data->width, shared_data->height, GL_RGBA, GL_FLOAT, rt::raytrace_get_data());
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, game_state.fbo);

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        bind_shader(game_state.voxel_shader);

        for (auto sub : game_state.voxel.sub_meshes)
        {
            glBindVertexArray(sub.vao_id);

            glm::mat4 trans = glm::scale(glm::mat4(1.0f), glm::vec3(1 / 16.0f, 1 / 16.0f, 1 / 16.0f));
            // trans = glm::translate(trans, glm::vec3(sub.x, sub.y, sub.z));
            uniform_mat4("u_transform", trans);
            glDrawArrays(GL_POINTS, 0, sub.voxel_count);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    {
        bind_shader(game_state.fbo_shader);
        glBindVertexArray(game_state.quad_vao);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, game_state.fbo_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}