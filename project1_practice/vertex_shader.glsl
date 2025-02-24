#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    // Flip the y-coordinate
    vec2 flippedPos = vec2(aPos.x, 600.0 - aPos.y);
    gl_Position = vec4(flippedPos / vec2(800.0, 600.0) * 2.0 - 1.0, 0.0, 1.0);
    TexCoord = aTexCoord;
}