
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

const float GAMMA = 2.5;
const float EXPOSURE = 10.0;
const float MIN_GAMMA = 0.000001;

uniform sampler2D u_map;
uniform sampler2D u_bloom;

void main() 
{ 
  // sample color from map
  vec3 result = texture(u_map, uvs).rgb + texture(u_bloom, uvs).rgb;

  // gamma correction
  result = pow(result, vec3(GAMMA));
  result = vec3(1.0) - exp(-result * EXPOSURE); 
  result = pow(result, vec3(1.0 / max(GAMMA, MIN_GAMMA)));

  // fragment color
  out_fragment = vec4(result, 1.0); 
}

++FRAGMENT++