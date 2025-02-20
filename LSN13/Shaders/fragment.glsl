#version 330 core

out vec4 FragColor;
flat in int textured;
in vec2 TexCoord;
in vec3 normal;
in vec3 fragment_position;

uniform vec4 set_color;
uniform sampler2D our_texture;

struct PointLight {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct DirectionalLight {
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

uniform PointLight point_light;
uniform DirectionalLight directional_light;
uniform vec4 view_position;
uniform float ambient_strength;

vec4 CalcPointLight(PointLight light, vec3 frag_pos, vec3 normal, vec3 view_dir);
vec4 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir);

void main()
{
    vec3 n = normalize(normal);
    vec3 view_dir = normalize(view_position.xyz - fragment_position);
    vec4 point_light_color = CalcPointLight(point_light, fragment_position, n, view_dir);
    vec4 directional_light_color = CalcDirectionalLight(directional_light, n, view_dir);

    vec4 total_light_color = point_light_color + directional_light_color;

    if (textured > 0) {
        FragColor = texture(our_texture, TexCoord) * total_light_color;
    } else {
        FragColor = set_color * total_light_color;
    }
}

vec4 CalcPointLight(PointLight light, vec3 frag_pos, vec3 normal, vec3 view_dir)
{
    vec3 light_dir = normalize(light.position.xyz - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);

    float diff_strength = max(dot(normal, light_dir), 0.0);
    float spec_strength = pow(max(dot(view_dir, reflect_dir), 0.0), 256);

    vec4 ambient = light.ambient * ambient_strength;
    vec4 diffuse = light.diffuse * diff_strength;
    vec4 specular = light.specular * spec_strength;

    return ambient + diffuse + specular;
}

vec4 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_dir = normalize(-light.direction);
    vec3 reflect_dir = reflect(-light_dir, normal);

    float diff_strength = max(dot(normal, light_dir), 0.0);
    float spec_strength = pow(max(dot(view_dir, reflect_dir), 0.0), 256);

    vec4 ambient = light.ambient * ambient_strength;
    vec4 diffuse = light.diffuse * diff_strength;
    vec4 specular = light.specular * spec_strength;

    return ambient + diffuse + specular;
}