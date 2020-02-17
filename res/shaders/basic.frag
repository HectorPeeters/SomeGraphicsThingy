#version 430 core

out vec4 frag_color;
  
in VS_OUT 
{
    vec3 vertex_pos;
    vec2 vertex_texture;
    mat3 TBN;
} fs_in;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int shininess;
};

struct Light 
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material u_material;

uniform Light u_light;

uniform vec3 u_cameraPos;

uniform sampler2D u_texture;
uniform sampler2D u_normal_texture;

void main()
{
    vec3 normal = texture(u_normal_texture, fs_in.vertex_texture).rgb;
    normal = (normal * 2.0 - 1.0);
    normal.y = -normal.y;
    normal = fs_in.TBN * normal;

    vec3 ambient = u_light.ambient * u_material.ambient; // * light_color

    vec3 to_light_vector =  fs_in.TBN * normalize(u_light.position - fs_in.vertex_pos);

    float diff = max(dot(normal, to_light_vector), 0.01);

    vec4 material_diffuse = texture(u_texture, fs_in.vertex_texture);
    vec4 diffuse = vec4(u_light.diffuse, 1.0) * (diff * material_diffuse); // u_material.diffuse

    // vec3 view_dir = normalize(u_cameraPos - vertex_pos);
    // vec3 reflect_dir = reflect(-to_light_vector, norm);
    // float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_material.shininess);
    // vec3 specular = u_light.ambient * (spec * u_material.specular);

    // frag_color = diffuse * diff;//vec4(ambient, 1.0) + diffuse;// + vec4(specular, 1.0);
    frag_color = diffuse;//vec4(diff, diff, diff, 1.0);//vec4(diff, diff, diff, 1.0);

    // float gamma = 2.2;
    // frag_color.rgb = pow(frag_color.rgb, vec3(1.0 / gamma));
} 