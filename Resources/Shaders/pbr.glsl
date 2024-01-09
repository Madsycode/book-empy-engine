#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uvs;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;

uniform mat4 u_model;
uniform mat4 u_proj;
uniform mat4 u_view;

out Vertex
{
  vec3 Position;
  vec3 Normal;
  mat3 TBN;
  vec2 UVs;
} vertex;
 
void main() 
{       
  vertex.UVs = a_uvs;
  vertex.Normal = (u_model * vec4(a_normal, 1.0)).xyz;
  vertex.Position = (u_model * vec4(a_position, 1.0)).xyz;
  gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
  vertex.TBN = mat3(u_model) * mat3(a_tangent, a_bitangent, a_normal);
}

++VERTEX++

#version 330 core
layout (location = 0) out vec4 out_fragment;

// constants
#define PI 3.14159265358979323846
#define MAX_LIGHTS 10

// direct light type
struct DirectLight
{
  float Intensity;
  vec3 Direction;
  vec3 Radiance;
};

// point light type
struct PointLight
{
  float Intensity;
  vec3 Position;
  vec3 Radiance;
};

// spot light type
struct SpotLight
{
  float Intensity;
  vec3 Direction;
  vec3 Position;
  vec3 Radiance;
  float FallOff;
  float CutOff;
};

// material type
struct Material
{
  sampler2D RoughnessMap;
  bool UseRoughnessMap;

  sampler2D MetallicMap;
  bool UseMetallicMap;

  sampler2D NormalMap;
  bool UseNormalMap;

  sampler2D AlbedoMap;
  bool UseAlbedoMap;

  float Roughness;
  float Metallic;
  vec3 Albedo;
};

// input vertex
in Vertex
{
  vec3 Position;
  vec3 Normal;
  mat3 TBN; // <-- added
  vec2 UVs;
} vertex;

// point light uniforms
uniform DirectLight u_directLights[MAX_LIGHTS]; 
uniform int u_nbrDirectLight; 

// point light uniforms
uniform PointLight u_pointLights[MAX_LIGHTS]; 
uniform int u_nbrPointLight; 

// spot light uniforms
uniform SpotLight u_spotLights[MAX_LIGHTS]; 
uniform int u_nbrSpotLight; 

// auxiliary uniforms
uniform Material u_material; 
uniform vec3 u_viewPos;

// enviroment maps
uniform samplerCube u_prefilMap; 
uniform samplerCube u_irradMap; 
uniform sampler2D u_brdfMap; 

// computes fresnel reflectivity
vec3 FresnelSchlick(float cosTheta, vec3 F0) 
{
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// computes geometry sclick ggx
float GeometrySchlickGGX(float NdotV, float roughness) 
{
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;
  float num = NdotV;
  float denom = NdotV * (1.0 - k) + k;
  return num / denom;
}

// computes distribution ggx
float DistributionGGX(vec3 N, vec3 H, float roughness) 
{
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;
  float num = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;
  return num / denom;
}

// computes geometry smith ggx
float GeometrySmithGGX(float NdotV, float NdotL, float roughness) 
{
  float ggx1 = GeometrySchlickGGX(NdotV, roughness);
  float ggx2 = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}

// compute ambient lights
vec3 ComputeAmbientLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic) 
{
 // angle between surface normal and light direction.
	float cosTheta = max(0.0, dot(N, V));
	
	// get diffuse contribution factor 
	vec3 F = FresnelSchlick(cosTheta, F0);
	vec3 Kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

	// irradiance map contains exitant radiance 
	vec3 diffuseIBL = Kd * albedo * texture(u_irradMap, N).rgb;

	// sample pre-filtered map at correct mipmap level.
	int mipLevels = 5;
	vec3 Lr = 2.0 * cosTheta * N - V;
	vec3 Ks = textureLod(u_prefilMap, Lr, roughness * mipLevels).rgb;

	// split-sum approx.factors for Cook-Torrance specular BRDF.
	vec2 brdf = texture(u_brdfMap, vec2(cosTheta, roughness)).rg;
	vec3 specularIBL = (F0 * brdf.x + brdf.y) * Ks;

	return (diffuseIBL + specularIBL);	
}

// compute direct lights
vec3 ComputeDirectLights(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic) 
{
  vec3 result = vec3(0.0);

  for (int i = 0; i < u_nbrDirectLight; ++i) 
  {
    // compute parameters
    vec3 L = -normalize(u_directLights[i].Direction);
    float NdotL = max(dot(N, L), 0.0);       
    float NdotV = max(dot(N, V), 0.0);    
    vec3 H = normalize(L + V);

    // Cook-Torrance (BRDF)
    float NDF = DistributionGGX(N, H, roughness);   
    vec3 FS = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    float GS = GeometrySmithGGX(NdotV, NdotL, roughness);      

    // diffuse light
    vec3 diffuse = (vec3(1.0) - FS) * (1.0 - metallic);

    // specular light
    vec3 specular = (NDF * GS * FS) / max(4.0 * NdotV * NdotL, 0.0001); 
    
    // combine components
    result += (diffuse * albedo / PI + specular) * u_directLights[i].Radiance * 
    NdotL * u_directLights[i].Intensity; 

    // break if max light
    if (i >= MAX_LIGHTS - 1) { break; }
  }

  return result;
}

// compute point lights
vec3 ComputePointLights(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic) 
{
  vec3 result = vec3(0.0);

  for (int i = 0; i < u_nbrSpotLight; ++i) 
  {
    // compute parameters
    vec3 L = normalize(u_pointLights[i].Position - vertex.Position);
    float NdotL = max(dot(N, L), 0.0);       
    float NdotV = max(dot(N, V), 0.0);    
    vec3 H = normalize(L + V);

    // Cook-Torrance (BRDF)
    float NDF = DistributionGGX(N, H, roughness);   
    vec3 FS = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    float GS = GeometrySmithGGX(NdotV, NdotL, roughness);      

    // diffuse light
    vec3 diffuse = (vec3(1.0) - FS) * (1.0 - metallic);

    // specular light
    vec3 specular = (NDF * GS * FS) / max(4.0 * NdotV * NdotL, 0.0001); 

    // light attenuation
    float distance = length(u_pointLights[i].Position - vertex.Position);
    float attenuation = u_pointLights[i].Intensity / (distance * distance); 

    // combine components
    result += (diffuse * albedo / PI + specular) * u_pointLights[i].Radiance * 
    attenuation * NdotL; 

    // break if max light
    if (i >= MAX_LIGHTS - 1) { break; }
  }

  return result;
}

// compute spot lights
vec3 ComputeSpotLights(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic) 
{
  vec3 result = vec3(0.0);

  for (int i = 0; i < u_nbrSpotLight; ++i) 
  {
    // compute parameters
    vec3 L = normalize(u_spotLights[i].Position - vertex.Position);
    float NdotL = max(dot(N, L), 0.0);       
    float NdotV = max(dot(N, V), 0.0);    
    vec3 H = normalize(L + V);

    // Cook-Torrance (BRDF)
    float NDF = DistributionGGX(N, H, roughness);   
    vec3 FS = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    float GS = GeometrySmithGGX(NdotV, NdotL, roughness);      

    // diffuse light
    vec3 diffuse = (vec3(1.0) - FS) * (1.0 - metallic);

    // specular light
    vec3 specular = (NDF * GS * FS) / max(4.0 * NdotV * NdotL, 0.0001); 

    // compute spot
    float theta = dot(L, normalize(-u_spotLights[i].Direction));
    float epsilon = (u_spotLights[i].FallOff - u_spotLights[i].CutOff);
    float spotFactor = clamp((theta - u_spotLights[i].CutOff)/epsilon, 0.0, 1.0);
    
    // light attenuation
    float distance = length(u_spotLights[i].Position - vertex.Position);
    float attenuation = u_spotLights[i].Intensity / (distance * distance); 

    // combine components
    result += (diffuse * albedo / PI + specular) * u_spotLights[i].Radiance * 
    u_spotLights[i].Intensity * attenuation * NdotL * spotFactor; 

    // break if max light
    if (i >= MAX_LIGHTS - 1) { break; }
  }

  return result;
}

// main function
void main() 
{
  // camera view direction
  vec3 V = normalize(u_viewPos - vertex.Position);

  // surface normal
  vec3 N = normalize(vertex.Normal);
  if(u_material.UseNormalMap) 
  { 
    // convert from [0,1] range to [-1, 1] range
    N = 2.0 * texture(u_material.NormalMap, vertex.UVs).rgb - 1.0;
    N = normalize(vertex.TBN * N); 
  }

  // material roughness
  float roughness = u_material.Roughness;
  if(u_material.UseRoughnessMap)
  {
    roughness = texture(u_material.RoughnessMap, vertex.UVs).r;
  }

  // material metallic
  float metallic = u_material.Metallic;
  if(u_material.UseMetallicMap)
  {
    metallic = texture(u_material.MetallicMap, vertex.UVs).r;
  }

  // material albedo 
  vec3 albedo = u_material.Albedo;
  if(u_material.UseAlbedoMap)
  {
    albedo = texture(u_material.AlbedoMap, vertex.UVs).rgb;
  }

  // fresnel reflectivity
  vec3 F0 = mix(vec3(0.04), albedo, metallic);  

  // lights contribution
  vec3 result = ComputeAmbientLight(N, V, F0, albedo, roughness, metallic);
  result += ComputeDirectLights(N, V, F0, albedo, roughness, metallic);
  result += ComputePointLights(N, V, F0, albedo, roughness, metallic);
  result += ComputeSpotLights(N, V, F0, albedo, roughness, metallic);

  // final color calculation
  out_fragment = vec4(result, 1.0);
} 

++FRAGMENT++