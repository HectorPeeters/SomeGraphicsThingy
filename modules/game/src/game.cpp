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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* The unknown key */
#define KEY_UNKNOWN -1

/* Printable keys */
#define KEY_SPACE 32
#define KEY_APOSTROPHE 39 /* ' */
#define KEY_COMMA 44      /* , */
#define KEY_MINUS 45      /* - */
#define KEY_PERIOD 46     /* . */
#define KEY_SLASH 47      /* / */
#define KEY_0 48
#define KEY_1 49
#define KEY_2 50
#define KEY_3 51
#define KEY_4 52
#define KEY_5 53
#define KEY_6 54
#define KEY_7 55
#define KEY_8 56
#define KEY_9 57
#define KEY_SEMICOLON 59 /* ; */
#define KEY_EQUAL 61     /* = */
#define KEY_A 65
#define KEY_B 66
#define KEY_C 67
#define KEY_D 68
#define KEY_E 69
#define KEY_F 70
#define KEY_G 71
#define KEY_H 72
#define KEY_I 73
#define KEY_J 74
#define KEY_K 75
#define KEY_L 76
#define KEY_M 77
#define KEY_N 78
#define KEY_O 79
#define KEY_P 80
#define KEY_Q 81
#define KEY_R 82
#define KEY_S 83
#define KEY_T 84
#define KEY_U 85
#define KEY_V 86
#define KEY_W 87
#define KEY_X 88
#define KEY_Y 89
#define KEY_Z 90
#define KEY_LEFT_BRACKET 91  /* [ */
#define KEY_BACKSLASH 92     /* \ */
#define KEY_RIGHT_BRACKET 93 /* ] */
#define KEY_GRAVE_ACCENT 96  /* ` */
#define KEY_WORLD_1 161      /* non-US #1 */
#define KEY_WORLD_2 162      /* non-US #2 */

/* Function keys */
#define KEY_ESCAPE 256
#define KEY_ENTER 257
#define KEY_TAB 258
#define KEY_BACKSPACE 259
#define KEY_INSERT 260
#define KEY_DELETE 261
#define KEY_RIGHT 262
#define KEY_LEFT 263
#define KEY_DOWN 264
#define KEY_UP 265
#define KEY_PAGE_UP 266
#define KEY_PAGE_DOWN 267
#define KEY_HOME 268
#define KEY_END 269
#define KEY_CAPS_LOCK 280
#define KEY_SCROLL_LOCK 281
#define KEY_NUM_LOCK 282
#define KEY_PRINT_SCREEN 283
#define KEY_PAUSE 284
#define KEY_F1 290
#define KEY_F2 291
#define KEY_F3 292
#define KEY_F4 293
#define KEY_F5 294
#define KEY_F6 295
#define KEY_F7 296
#define KEY_F8 297
#define KEY_F9 298
#define KEY_F10 299
#define KEY_F11 300
#define KEY_F12 301
#define KEY_F13 302
#define KEY_F14 303
#define KEY_F15 304
#define KEY_F16 305
#define KEY_F17 306
#define KEY_F18 307
#define KEY_F19 308
#define KEY_F20 309
#define KEY_F21 310
#define KEY_F22 311
#define KEY_F23 312
#define KEY_F24 313
#define KEY_F25 314
#define KEY_KP_0 320
#define KEY_KP_1 321
#define KEY_KP_2 322
#define KEY_KP_3 323
#define KEY_KP_4 324
#define KEY_KP_5 325
#define KEY_KP_6 326
#define KEY_KP_7 327
#define KEY_KP_8 328
#define KEY_KP_9 329
#define KEY_KP_DECIMAL 330
#define KEY_KP_DIVIDE 331
#define KEY_KP_MULTIPLY 332
#define KEY_KP_SUBTRACT 333
#define KEY_KP_ADD 334
#define KEY_KP_ENTER 335
#define KEY_KP_EQUAL 336
#define KEY_LEFT_SHIFT 340
#define KEY_LEFT_CONTROL 341
#define KEY_LEFT_ALT 342
#define KEY_LEFT_SUPER 343
#define KEY_RIGHT_SHIFT 344
#define KEY_RIGHT_CONTROL 345
#define KEY_RIGHT_ALT 346
#define KEY_RIGHT_SUPER 347
#define KEY_MENU 348

#define KEY_LAST KEY_MENU

#define PI 3.141592653689

#define EXPORT_METHOD extern "C"

struct GameState
{
    unsigned int basic_shader;
    unsigned int uniform_count;
    unsigned int attribute_count;
    char uniform_names[64][64];
    char attribute_names[64][64];

    unsigned int fbo_shader;
    unsigned int fbo;
    unsigned int fbo_rbo;
    unsigned int fbo_texture;

    unsigned int quad_vao;
    unsigned int quad_vertices_vbo;
    unsigned int quad_texture_vbo;

    glm::mat4 view_matrix;

    unsigned int transfom_loc;
    unsigned int projection_loc;
    unsigned int view_loc;

    unsigned int camera_pos_loc;

    unsigned int light_pos_loc;
    unsigned int light_ambient_loc;
    unsigned int light_diffuse_loc;
    unsigned int light_specular_loc;

    unsigned int material_ambient_loc;
    unsigned int material_diffuse_loc;
    unsigned int material_specular_loc;
    unsigned int material_shininess_loc;

    unsigned int exposure_loc;
    float exposure = 1.0;

    unsigned int mesh_vertex_vbo;
    unsigned int mesh_texture_vbo;
    unsigned int mesh_normal_vbo;
    unsigned int mesh_tangent_vbo;
    unsigned int mesh_bitangent_vbo;

    unsigned int mesh_vao;
    unsigned int mesh_ibo;

    unsigned int texture;
    unsigned int normal_texture;

    float model_scale = 1;

    glm::vec3 light_position;
    glm::vec3 light_ambient{0.0f, 0.0f, 0.0f};
    glm::vec3 light_diffuse{0.5f, 0.5f, 0.5f};
    glm::vec3 light_specular{0.5f, 0.5f, 0.5f};

    glm::vec3 material_ambient{0.5f, 0.5f, 0.5f};
    glm::vec3 material_diffuse{0.5f, 0.5f, 0.5f};
    glm::vec3 material_specular{0.5f, 0.5f, 0.5f};
    int material_shininess = 32;

    glm::vec3 camera_pos{0.0f, 0.0f, 3.0f};
    glm::vec3 camera_front{0.0f, 0.0f, -1.0f};
    glm::vec3 camera_up{0.0f, 1.0f, 0.0f};

    double last_mouse_x = -1;
    double last_mouse_y = -1;

    float pitch = 0;
    float yaw = -90;
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

unsigned int load_shader_program(const char *vertex_path, const char *fragment_path)
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

    unsigned int program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program_id, 512, NULL, info_log);
        fprintf(stderr, "Shader Error: Failed to link shader program %s, %s: %s\n", vertex_path, fragment_path, info_log);
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

unsigned int load_texture(const char *path)
{
    int width, height, num_channels;
    unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);

    if (!data)
    {
        fprintf(stderr, "Failed to load texture: %s\n", path);
        stbi_image_free(data);
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

void load_quad_mesh()
{
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };

    float texture[] = {
        1.0f, 0.0f, //
        1.0f, 1.0f, //
        0.0f, 1.0f, //
        0.0f, 0.0f, //
    };

    float normals[] = {
        0.0f, 0.0f, -1.0f, //
        0.0f, 0.0f, -1.0f, //
        0.0f, 0.0f, -1.0f, //
        0.0f, 0.0f, -1.0f, //
    };

    float tangents[] = {
        -1.0f, 0.0f, 0.0f, //
        0.0f, 1.0f, 0.0f,  //
        1.0f, 0.0f, 0.0f,  //
        0.0f, -1.0f, 0.0f  //
    };

    float bitangents[] = {
        0.0f, -1.0f, 0.0f, //
        -1.0f, 0.0f, 0.0f, //
        0.0f, 1.0f, 0.0f,  //
        1.0f, 0.0f, 0.0f,  //
    };

    unsigned int indices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    glGenVertexArrays(1, &game_state.mesh_vao);
    glGenBuffers(1, &game_state.mesh_vertex_vbo);
    glGenBuffers(1, &game_state.mesh_texture_vbo);
    glGenBuffers(1, &game_state.mesh_normal_vbo);
    glGenBuffers(1, &game_state.mesh_tangent_vbo);
    glGenBuffers(1, &game_state.mesh_bitangent_vbo);
    glGenBuffers(1, &game_state.mesh_ibo);

    glBindVertexArray(game_state.mesh_vao);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.mesh_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.mesh_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.mesh_normal_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.mesh_tangent_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, game_state.mesh_bitangent_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bitangents), bitangents, GL_STATIC_DRAW);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, game_state.mesh_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void delete_triangle_mesh()
{
    glDeleteVertexArrays(1, &game_state.mesh_vao);
    glDeleteBuffers(1, &game_state.mesh_vertex_vbo);
    glDeleteBuffers(1, &game_state.mesh_texture_vbo);
    glDeleteBuffers(1, &game_state.mesh_normal_vbo);
    glDeleteBuffers(1, &game_state.mesh_tangent_vbo);
    glDeleteBuffers(1, &game_state.mesh_bitangent_vbo);
    glDeleteBuffers(1, &game_state.mesh_ibo);
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

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

    // glm::mat4 trans = glm::mat4(1.0f);
    // trans = glm::translate(trans, glm::vec3(0, 0, -2));
    // trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    // trans = glm::scale(trans, glm::vec3(1.0, 1.0, 1.0));

    game_state.transfom_loc = glGetUniformLocation(game_state.basic_shader, "u_transform");
    glUseProgram(game_state.basic_shader);
    // glUniformMatrix4fv(game_state.transfom_loc, 1, GL_FALSE, glm::value_ptr(trans));

    game_state.projection_loc = glGetUniformLocation(game_state.basic_shader, "u_projection");
    game_state.view_loc = glGetUniformLocation(game_state.basic_shader, "u_view");
    game_state.camera_pos_loc = glGetUniformLocation(game_state.basic_shader, "u_cameraPos");

    game_state.light_pos_loc = glGetUniformLocation(game_state.basic_shader, "u_light.position");
    game_state.light_ambient_loc = glGetUniformLocation(game_state.basic_shader, "u_light.ambient");
    game_state.light_diffuse_loc = glGetUniformLocation(game_state.basic_shader, "u_light.diffuse");
    game_state.light_specular_loc = glGetUniformLocation(game_state.basic_shader, "u_light.specular");

    game_state.material_ambient_loc = glGetUniformLocation(game_state.basic_shader, "u_material.ambient");
    game_state.material_diffuse_loc = glGetUniformLocation(game_state.basic_shader, "u_material.diffuse");
    game_state.material_specular_loc = glGetUniformLocation(game_state.basic_shader, "u_material.specular");
    game_state.material_shininess_loc = glGetUniformLocation(game_state.basic_shader, "u_material.shininess");

    game_state.light_position.z = -1.95f;

    load_quad_mesh();

    game_state.texture = load_texture("res/textures/2k/Tiles32_col.jpg");
    game_state.normal_texture = load_texture("res/textures/2k/Tiles32_nrm.jpg");

    glUniform1i(glGetUniformLocation(game_state.basic_shader, "u_texture"), 0);
    glUniform1i(glGetUniformLocation(game_state.basic_shader, "u_normal_texture"), 1);

    glUseProgram(game_state.fbo_shader);

    game_state.exposure_loc = glGetUniformLocation(game_state.fbo_shader, "u_exposure");
    return true;
}

EXPORT_METHOD void deinit()
{
    glDeleteRenderbuffers(1, &game_state.fbo_rbo);
    glDeleteFramebuffers(1, &game_state.fbo);

    delete_triangle_mesh();
    delete_fbo_quad();

    delete_shader(game_state.basic_shader);
    delete_shader(game_state.fbo_shader);

    glDeleteTextures(1, &game_state.texture);
    glDeleteTextures(1, &game_state.normal_texture);
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

    ImGui::SliderFloat("Scale", &game_state.model_scale, 1, 20);
    ImGui::SliderFloat("Exposure", &game_state.exposure, 0, 5);

    if (ImGui::CollapsingHeader("Lighting"))
    {
        ImGui::SliderFloat("X", &game_state.light_position.x, -1, 1);
        ImGui::SliderFloat("Y", &game_state.light_position.y, -1, 1);
        ImGui::SliderFloat("Z", &game_state.light_position.z, -1, -1.999);

        ImGui::SliderFloat3("Light Ambient", glm::value_ptr(game_state.light_ambient), 0, 1);
        ImGui::SliderFloat3("Light Diffuse", glm::value_ptr(game_state.light_diffuse), 0, 10);
        ImGui::SliderFloat3("Light Specular", glm::value_ptr(game_state.light_specular), 0, 1);
    }

    if (ImGui::CollapsingHeader("Material"))
    {
        ImGui::SliderFloat3("Mat Ambient", glm::value_ptr(game_state.material_ambient), 0, 1);
        ImGui::SliderFloat3("Mat Diffuse", glm::value_ptr(game_state.material_diffuse), 0, 10);
        ImGui::SliderFloat3("Mat Specular", glm::value_ptr(game_state.material_specular), 0, 1);
        ImGui::SliderInt("Light Shininess", &game_state.material_shininess, 0, 100);
    }

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
}

float i = 0;

void update_camera()
{
    const float camera_speed = 0.05f;

    if (shared_data->keys[KEY_W])
        game_state.camera_pos += camera_speed * game_state.camera_front;
    if (shared_data->keys[KEY_S])
        game_state.camera_pos -= camera_speed * game_state.camera_front;

    if (shared_data->keys[KEY_A])
        game_state.camera_pos -= glm::normalize(glm::cross(game_state.camera_front, game_state.camera_up)) * camera_speed;
    if (shared_data->keys[KEY_D])
        game_state.camera_pos += glm::normalize(glm::cross(game_state.camera_front, game_state.camera_up)) * camera_speed;

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
        const float mouse_sensitivity = 0.05f;
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

EXPORT_METHOD void update(float delta)
{
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)shared_data->width / (float)shared_data->height, 0.1f, 1000.0f);
    glUseProgram(game_state.basic_shader);
    glUniformMatrix4fv(game_state.projection_loc, 1, GL_FALSE, glm::value_ptr(proj));

    game_state.light_position.y = sin(i) / 3.0f;
    game_state.light_position.x = cos(i) / 3.0f;
    i += delta;

    update_camera();

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

    glUniformMatrix4fv(game_state.view_loc, 1, GL_FALSE, glm::value_ptr(game_state.view_matrix));

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0, 0, 0));
    // trans = glm::rotate(trans, glm::radians(-0.0f), glm::vec3(1.0, 0.0, 0.0));
    trans = glm::scale(trans, glm::vec3(game_state.model_scale));
    glUniformMatrix4fv(game_state.transfom_loc, 1, GL_FALSE, glm::value_ptr(trans));

    glUseProgram(game_state.fbo_shader);
    glUniform1f(game_state.exposure_loc, game_state.exposure);
}

EXPORT_METHOD void render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, game_state.fbo);
    {
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, game_state.texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, game_state.normal_texture);

        glUseProgram(game_state.basic_shader);
        glBindVertexArray(game_state.mesh_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    {
        glUseProgram(game_state.fbo_shader);
        glBindVertexArray(game_state.quad_vao);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, game_state.fbo_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
