#version 430 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texture;

out vec2 vertex_texture;
out vec3 vertex_pos;

uniform mat4 u_projection;
uniform mat4 u_transform;

void main()
{
    gl_Position = u_projection * u_transform * vec4(a_position, 1.0);
    vertex_texture = a_texture;
    vertex_pos = (u_transform * vec4(a_position, 1.0)).xyz;
}
