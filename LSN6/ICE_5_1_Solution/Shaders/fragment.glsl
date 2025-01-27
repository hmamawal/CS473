#version 330 core

out vec4 FragColor;

flat in int textured;

in vec2 TexCoord;

uniform vec4 set_color;
uniform sampler2D our_texture;

void main()
{
    if (textured > 0)
    {
        FragColor = texture(our_texture, TexCoord);
        return;
    }
    FragColor = set_color;
}