#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform bool is_textured;

flat out int textured;
out vec2 TexCoord;
out vec3 normal;
out vec3 fragment_position;

uniform mat4 projection, view, model, local;


void main()
{
    if (is_textured) {
        textured = 1;
        TexCoord = aTexCoord;

    } else {
        textured = 0;
    }
    normal = mat3(transpose(inverse(local))) * aNormal;
    fragment_position = vec3(model * local * vec4(aPos,1.0));
    gl_Position = projection * view * vec4(fragment_position,1.0);
}
