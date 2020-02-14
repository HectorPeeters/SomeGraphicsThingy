#version 430 core

in vec2 tex_coord;

out vec4 frag_color;

uniform sampler2D fbo;

void main()
{
    frag_color = texture(fbo, tex_coord);
}