#version 430 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

out vec3 vertex_color;
out vec3 vertex_pos;

uniform mat4 u_projection;
uniform mat4 u_transform;

void main()
{
    gl_Position = u_projection * u_transform * vec4(a_position, 1.0);
    vertex_color = a_color;
    vertex_pos = (u_transform * vec4(a_position, 1.0)).xyz;
}
