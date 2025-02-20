#version 330 core

out vec4 FragColor;
flat in int textured;
in vec2 TexCoord;
in vec3 normal;
in vec3 fragment_position;

uniform vec4 set_color;

uniform sampler2D our_texture;

uniform vec4 light_color;
uniform vec4 light_position;
uniform vec4 view_position;
uniform float ambient_strength;

void main()
{
    vec3 light_direction = normalize(light_position.xyz - fragment_position);
    vec3 view_direction = normalize(view_position.xyz - fragment_position);
    
    vec3 n = normalize(normal);
    vec3 reflect_direction = reflect(-light_direction,n);
    float diff_strength = max(dot(n,light_direction),0.0);
    float spec_strength = pow(max(dot(view_direction,reflect_direction),0.0),256);
    if (textured > 0) {
        FragColor = texture(our_texture, TexCoord);
    } else {
        FragColor = set_color;
    }
    FragColor = (spec_strength + diff_strength + ambient_strength) * light_color * FragColor;
    
}