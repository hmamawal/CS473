#version 330 core

out vec4 FragColor;
flat in int textured;
in vec2 TexCoord;
in vec3 normal;
in vec3 fragment_position;

uniform vec4 set_color;

uniform sampler2D our_texture;


struct DirectionalLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 direction;
    bool on;
};


uniform vec4 view_position;
uniform DirectionalLight directional_light;

vec4 CalcDirectionalLight();


void main()
{
    
    if (textured > 0) {
        FragColor = texture(our_texture, TexCoord);
    } else {
        FragColor = set_color;
    }
    vec3 light_color = CalcDirectionalLight().xyz;
    FragColor = vec4(light_color * FragColor.rgb,FragColor.a);
    
}

vec4 CalcDirectionalLight () {
    if(!(directional_light.on)){
        return vec4(0.0,0.0,0.0,1.0);
    }
    vec3 light_direction = normalize(-(directional_light.direction.xyz));
    vec3 view_direction = normalize(view_position.xyz - fragment_position);
    vec3 n = normalize(normal);
    vec3 reflect_direction = reflect(-light_direction,n);
    float diff_strength = max(dot(n,light_direction),0.0);
    float spec_strength = pow(max(dot(view_direction,reflect_direction),0.0),256);
    vec4 light_color = directional_light.ambient;
    light_color += diff_strength * directional_light.diffuse;
    light_color += spec_strength * directional_light.specular;
    return light_color;
}
