#version 430 core
out vec4 frag_color;
  
in vec2 vertex_texture;
in vec3 vertex_pos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int shininess;
};

uniform Material u_material;

struct Light 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light u_light;

uniform vec3 u_cameraPos;

uniform sampler2D u_texture;

void main()
{
    vec3 ambient = u_light.ambient * u_material.ambient; // * light_color;
    
    vec3 norm = normalize(vec3(0, 0, -1));
    vec3 to_light_vector = normalize(u_light.position - vertex_pos);
    float diff = max(dot(norm, to_light_vector), 0.1);
    vec4 material_diffuse = texture(u_texture, vertex_texture);
    vec4 diffuse = vec4(u_light.diffuse, 1.0) * (diff * material_diffuse); // u_material.diffuse

    vec3 view_dir = normalize(u_cameraPos - vertex_pos);
    vec3 reflect_dir = reflect(-to_light_vector, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_material.shininess);
    vec3 specular = u_light.ambient * (spec * u_material.specular);

    frag_color = vec4(ambient, 1.0) + diffuse + vec4(specular, 1.0);

    float gamma = 2.2;
    frag_color.rgb = pow(frag_color.rgb, vec3(1.0 / gamma));
} 