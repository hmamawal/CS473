#version 330 core
//Need to send the fragment color to the next step in the pipeline
out vec4 FragColor;

//bring in the texture image coordinates (if you have them)
in vec2 texture_coordinates;

//bring in the normal and the fragment position (world)
in vec3 norm;
in vec3 fragment_position;

//bring in the imported information
in vec3 ambient_color;
in vec3 diffuse_color;
in vec3 specular_color;
flat in float opacity;

//bring in the integer value (0 or 1) telling us if the shape includes
//  texture data or not.
flat in int textured;
flat in int imported;
//bring in the index for the texture
flat in uint index_for_texture;

//if it isn't textured, assume the value of set_color is set.
uniform vec4 set_color;

//if it -is- textured, assume this 2D texture is mapped.
#define NR_TEXTURES 16
uniform sampler2D textures[NR_TEXTURES];

// uniform float ambient_strength;
// uniform vec4 light_color;
// uniform vec4 light_position;
uniform vec4 view_position;

struct PointLight {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    //add coefficients for attenuation here
    bool on;
};

uniform PointLight point_light;

vec4 CalcPointLight (PointLight light,vec3 norm,vec3 frag,vec3 eye);

bool use_texture = (textured == 1) || ((imported == 1) && (1.0*index_for_texture < 1.0*99));
bool use_material = (imported == 1) && (1.0*index_for_texture > 1.0*98);
bool use_set_color = !use_texture && !use_material;

void main()
{
    vec4 point_light_color = CalcPointLight(point_light,norm,
                                            fragment_position,view_position.xyz);
    if (use_material) {
        FragColor = point_light_color;
        return;
    }

    if (use_texture) {
        FragColor = point_light_color * texture(textures[index_for_texture],texture_coordinates);
    return;
    }

    FragColor = point_light_color * set_color;
    
};

vec4 CalcPointLight (PointLight light,vec3 norm,vec3 frag,vec3 eye) {

    if (!light.on) {
        return vec4(0.0,0.0,0.0,1.0);
    }
    vec3 light_direction = light.position.xyz - frag.xyz;
    light_direction = normalize(light_direction);
    vec3 normal = normalize(norm);
    float diffuse_coeff = max(dot(normal,light_direction),0.0);

    vec3 view_direction = normalize(view_position.xyz-frag);
    vec3 reflect_direction = reflect(-light_direction,normal);
    float spec_coeff = pow(max(dot(view_direction,reflect_direction),0.0),256.0);

    //handle materials
    if (use_material) {
        return (light.ambient * vec4(ambient_color,opacity) 
                + diffuse_coeff * light.diffuse * vec4(diffuse_color,opacity)
                + spec_coeff * light.specular * vec4(diffuse_color,opacity));
    } else {
        return (light.ambient 
            + diffuse_coeff * light.diffuse 
            + spec_coeff * light.specular);
    }




}