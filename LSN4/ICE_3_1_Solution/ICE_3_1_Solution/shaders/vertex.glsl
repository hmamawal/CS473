#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertex_color;

uniform vec4 offset;

void main()
{
   // could do this
   // gl_position = vec4(aPos.sxz+offset.xyz, 1.0);
   gl_Position = vec4(aPos.x+offset.x, aPos.y+offset.y, aPos.z, 1.0);
   vertex_color = aColor;
};