#version 430 core

in vec2 tex_coord;

out vec4 frag_color;

uniform sampler2D fbo;

uniform float u_exposure;
uniform float u_gamma;

void main()
{
    vec3 hdr_color = texture(fbo, tex_coord).rgb;

    vec3 mapped = vec3(1.0) - exp(-hdr_color * u_exposure);

    mapped = pow(mapped, vec3(1.0 / u_gamma));

    frag_color = vec4(mapped, 1.0);
}