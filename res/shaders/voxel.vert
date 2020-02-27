#version 430 core

layout (location = 0) in vec4 a_data;

uniform mat4 u_transform;
uniform mat4 u_view;
uniform mat4 u_projection;

out flat int out_color;

void main()
{
    gl_Position = u_projection * u_view * u_transform * vec4(a_data.xyz, 1.0);

    gl_PointSize = 105 / gl_Position.w;

    out_color = int(a_data.w);
}