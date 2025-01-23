#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec4 offset_vec;


void main()
{
    gl_Position = vec4(aPos.x+offset_vec.x, aPos.y+offset_vec.y, aPos.z, 1.0);
}
