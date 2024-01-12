#version 330 core
layout (location = 0) in vec3 a_position;

uniform mat4 u_lightSpace;
uniform mat4 u_model;

void main() 
{
  gl_Position = u_lightSpace * u_model * vec4(a_position, 1.0f);
}

++VERTEX++

#version 330 core

void main() 
{ 
  // gl_Depth = ...
}

++FRAGMENT++