
#version 330 core
layout (location = 0) in vec4 a_quad;

out vec2 uvs;

void main() 
{
  gl_Position = vec4(a_quad.x, a_quad.y, 0.0, 1.0); 
  uvs = vec2(a_quad.z, a_quad.w);
}

++VERTEX++

#version 330 core
out vec4 out_fragment;

in vec2 uvs;

uniform sampler2D u_map;

void main() 
{ 
  out_fragment = texture(u_map, uvs);
}

++FRAGMENT++