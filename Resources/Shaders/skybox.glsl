#version 330 core
layout (location = 0) in vec3 a_position;

out vec3 world_position;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

void main() 
{
  vec4 position = u_proj * mat4(mat3(u_view))  * 
  u_model * vec4(a_position, 1.0f);

  gl_Position = position.xyww;
  world_position = a_position;
}

++VERTEX++

#version 330 core
out vec4 out_fragment;

in vec3 world_position;
uniform samplerCube u_map;

void main() 
{ 
  out_fragment = vec4(texture(u_map, world_position).rgb, 1.0);  
}

++FRAGMENT++