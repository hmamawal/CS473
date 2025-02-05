#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform bool is_textured;

flat out int textured;
out vec2 TexCoord;

uniform mat4 projection, view, model, local;


void main()
{
    if (is_textured) {
        textured = 1;
        TexCoord = aTexCoord;

    } else {
        textured = 0;
    }
    gl_Position = projection * view * model * local * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
