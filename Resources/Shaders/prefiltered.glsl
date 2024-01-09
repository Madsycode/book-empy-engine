#version 330 core
layout (location = 0) in vec3 a_position;

out vec3 world_position;

uniform mat4 u_view;
uniform mat4 u_proj;

void main() 
{
  gl_Position = u_proj * u_view * vec4(a_position, 1.0f);
  world_position = a_position;
}

++VERTEX++

#version 330 core
out vec4 out_fragment; 
in vec3 world_position;

const float PI = 3.14159265358979323846;

uniform float u_roughness;
uniform samplerCube u_cubemap;

float RadicalInverseVdC(uint bits)  
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) 
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  

vec2 Hammersley(uint i, uint N) 
{
    return vec2(float(i)/float(N), RadicalInverseVdC(i));
}  

void main() 
{		
    vec3 N = normalize(world_position);    
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 result = vec3(0.0);     

    for(uint i = 0u; i < SAMPLE_COUNT; ++i) 
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, u_roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0) 
        {
            result += texture(u_cubemap, L).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    result = result / totalWeight;
    out_fragment = vec4(result, 1.0);
}

++FRAGMENT++