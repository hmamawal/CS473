#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec4 offset_vec;

uniform bool is_textured;

flat out int textured;

out vec2 TexCoord;

void main()
{
    if (is_textured)
    {
        TexCoord = aTexCoord;
        textured = 1;
    }
    else
    {
        textured = 0;
    }
    gl_Position = vec4(aPos.x+offset_vec.x, aPos.y+offset_vec.y, aPos.z, 1.0);
}
