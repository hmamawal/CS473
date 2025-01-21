#version 330 core
in vec3 vertex_color;
out vec4 FragColor;

uniform vec4 set_color;
uniform bool use_color;

void main()
{
   if (use_color)
   {
      FragColor = set_color;
      return;
   }

   //FragColor = vec4(1.0,1.0,0.0,1.0);
   FragColor = vec4(vertex_color, 1.0);
}

