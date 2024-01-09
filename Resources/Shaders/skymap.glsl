#version 330 core
layout (location = 0) in vec3 a_position;

out vec3 world_position;

uniform mat4 u_view;
uniform mat4 u_proj;

void main() 
{
	gl_Position =  u_proj * u_view * vec4(a_position, 1.0);
  world_position = normalize(a_position);
}

++VERTEX++

#version 330 core
out vec4 out_fragment;
in vec3 world_position;

uniform sampler2D u_map;

vec2 GetSphericalUVs(vec3 v) 
{
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= vec2(0.1591, 0.3183);
  uv += 0.5;
  return uv;
}

void main() 
{ 
  vec2 uv = GetSphericalUVs(world_position); 
	vec3 color = texture(u_map, uv).rgb;
  out_fragment = vec4(color, 1.0f);
}

++FRAGMENT++