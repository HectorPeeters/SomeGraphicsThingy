#version 430 core

in flat int out_color;

uniform vec4 u_colors[256];

out vec4 frag_color;


void main()
{
    // frag_color = vec4(out_color, out_color, out_color, 1.0);
    // frag_color /= 255;
    // frag_color = vec4(1.0, 1.0, 1.0, 1.0);
    // frag_color = u_color + vec4(out_color) / 256.0;

    frag_color = u_colors[out_color];
}