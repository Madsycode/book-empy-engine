#pragma once
#include "Shader.h"

namespace Empy
{
    struct PbrShader : Shader 
    {        
        EMPY_INLINE PbrShader(const std::string& filename): Shader(filename) 
        {
            u_NbrDirectLight = glGetUniformLocation(m_ShaderID, "u_nbrDirectLight");
            u_NbrPointLight = glGetUniformLocation(m_ShaderID, "u_nbrPointLight");
            u_NbrSpotLight = glGetUniformLocation(m_ShaderID, "u_nbrSpotLight");

            u_PrefilMap = glGetUniformLocation(m_ShaderID, "u_prefilMap");            
            u_IrradMap = glGetUniformLocation(m_ShaderID, "u_irradMap");
            u_BrdfMap = glGetUniformLocation(m_ShaderID, "u_brdfMap");

            u_LightSpace = glGetUniformLocation(m_ShaderID, "u_lightSpace");
            u_DepthMap = glGetUniformLocation(m_ShaderID, "u_depthMap");

            u_HasJoints = glGetUniformLocation(m_ShaderID, "u_hasJoints");

            u_ViewPos = glGetUniformLocation(m_ShaderID, "u_viewPos");
            u_Model = glGetUniformLocation(m_ShaderID, "u_model");
            u_View = glGetUniformLocation(m_ShaderID, "u_view");
            u_Proj = glGetUniformLocation(m_ShaderID, "u_proj");
            
            u_Material.Initialize(m_ShaderID);
        } 

        EMPY_INLINE void SetEnvMaps(uint32_t irrad, uint32_t prefil, uint32_t brdf, uint32_t depthMap)
        {
            glUseProgram(m_ShaderID);
            
            // irradiance map
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irrad);
            glUniform1i(u_IrradMap, 0);

            // prefiltered map
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefil);
            glUniform1i(u_PrefilMap, 1);

            // BRDF Map
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, brdf);
            glUniform1i(u_BrdfMap, 2);

            // Depth Map
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(u_DepthMap, 3);
        }

        EMPY_INLINE void SetDirectLight(DirectLight& light, Transform3D& transform, int32_t index) 
        {
            std::string intensity = "u_directLights[" + std::to_string(index) + "].Intensity";
            std::string direction = "u_directLights[" + std::to_string(index) + "].Direction";
            std::string radiance = "u_directLights[" + std::to_string(index) + "].Radiance";

            uint32_t u_intensity = glGetUniformLocation(m_ShaderID, intensity.c_str());
            uint32_t u_direction = glGetUniformLocation(m_ShaderID, direction.c_str());
            uint32_t u_radiance = glGetUniformLocation(m_ShaderID, radiance.c_str());

            glUniform3fv(u_direction, 1, &transform.Rotation.x);
            glUniform3fv(u_radiance, 1, &light.Radiance.x);
            glUniform1f(u_intensity, light.Intensity);
        }

        EMPY_INLINE void SetPointLight(PointLight& light, Transform3D& transform, int32_t index) 
        {
            std::string intensity = "u_pointLights[" + std::to_string(index) + "].Intensity";
            std::string radiance = "u_pointLights[" + std::to_string(index) + "].Radiance";
            std::string position = "u_pointLights[" + std::to_string(index) + "].Position";

            uint32_t u_intensity = glGetUniformLocation(m_ShaderID, intensity.c_str());
            uint32_t u_radiance = glGetUniformLocation(m_ShaderID, radiance.c_str());
            uint32_t u_position = glGetUniformLocation(m_ShaderID, position.c_str());

            glUniform3fv(u_position, 1, &transform.Translate.x);
            glUniform3fv(u_radiance, 1, &light.Radiance.x);
            glUniform1f(u_intensity, light.Intensity);
        }

        EMPY_INLINE void SetSpotLight(SpotLight& light, Transform3D& transform, int32_t index) 
        {
            std::string intensity = "u_spotLights[" + std::to_string(index) + "].Intensity";
            std::string direction = "u_spotLights[" + std::to_string(index) + "].Direction";
            std::string radiance = "u_spotLights[" + std::to_string(index) + "].Radiance";
            std::string position = "u_spotLights[" + std::to_string(index) + "].Position";
            std::string falloff = "u_spotLights[" + std::to_string(index) + "].FallOff";
            std::string cutoff = "u_spotLights[" + std::to_string(index) + "].CutOff";

            uint32_t u_intensity = glGetUniformLocation(m_ShaderID, intensity.c_str());
            uint32_t u_direction = glGetUniformLocation(m_ShaderID, direction.c_str());
            uint32_t u_radiance = glGetUniformLocation(m_ShaderID, radiance.c_str());
            uint32_t u_position = glGetUniformLocation(m_ShaderID, position.c_str());
            uint32_t u_falloff = glGetUniformLocation(m_ShaderID, falloff.c_str());
            uint32_t u_cutoff = glGetUniformLocation(m_ShaderID, cutoff.c_str());

            glUniform3fv(u_direction, 1, &transform.Rotation.x);
            glUniform3fv(u_position, 1, &transform.Translate.x);
            glUniform3fv(u_radiance, 1, &light.Radiance.x);

            glUniform1f(u_falloff, glm::radians(light.FallOff));
            glUniform1f(u_cutoff, glm::radians(light.CutOff));
            glUniform1f(u_intensity, light.Intensity);
        }

        
        EMPY_INLINE void SetJoints(std::vector<glm::mat4>& joints) 
        {
            for (size_t i = 0; i < joints.size() && i < 100; ++i) 
            {
                std::string uniform = "u_joints[" + std::to_string(i) + "]";
                uint32_t u_joint = glGetUniformLocation(m_ShaderID, uniform.c_str());
                glUniformMatrix4fv(u_joint, 1, GL_FALSE, glm::value_ptr(joints[i]));
            }
        }

        EMPY_INLINE void Draw(Model3D& model, Material& mtl, Transform3D& transform)
        {
            // set transform
            glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));  
            glUniform1i(u_HasJoints, model->HasJoints()); 
            // set mtl
            SetMaterial(mtl, 4);
            // render mesh
            model->Draw(GL_TRIANGLES);        
        }

        EMPY_INLINE void SetCamera(Camera3D& camera, Transform3D& transform, float ratio) 
        {
            glUniformMatrix4fv(u_Proj, 1, GL_FALSE, glm::value_ptr(camera.Projection(ratio)));
            glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(camera.View(transform)));
            glUniform3fv(u_ViewPos, 1, &transform.Translate.x);
        } 

        EMPY_INLINE void SetLightSpaceMatrix(const glm::mat4& lightSpaceMtx)
        {
            // set view projection matrix
            glUniformMatrix4fv(u_LightSpace, 1, GL_FALSE, glm::value_ptr(lightSpaceMtx));  
        }

        EMPY_INLINE void SetDirectLightCount(int32_t count)
        {
            glUniform1i(u_NbrDirectLight, count);
        }

        EMPY_INLINE void SetPointLightCount(int32_t count)
        {
            glUniform1i(u_NbrPointLight, count);
        }

        EMPY_INLINE void SetSpotLightCount(int32_t count)
        {
            glUniform1i(u_NbrSpotLight, count);
        }

private:
        EMPY_INLINE void UseMap(uint32_t map, uint32_t uniform, int32_t unit) 
        { 
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, map);
            glUniform1i(uniform, unit); 
        }

        EMPY_INLINE void SetMaterial(Material& mtl, int32_t unit) 
		{
			// set usability
			glUniform1i(u_Material.UseRoughnessMap, mtl.RoughnessMap);
			glUniform1i(u_Material.UseOcclusionMap, mtl.OcclusionMap);
			glUniform1i(u_Material.UseEmissiveMap, mtl.EmissiveMap);
			glUniform1i(u_Material.UseMetallicMap, mtl.MetallicMap);
			glUniform1i(u_Material.UseAlbedoMap, mtl.AlbedoMap);
			glUniform1i(u_Material.UseNormalMap, mtl.NormalMap);

			// set mtl maps
            UseMap(mtl.RoughnessMap, u_Material.RoughnessMap, unit++);
            UseMap(mtl.OcclusionMap, u_Material.OcclusionMap, unit++);
            UseMap(mtl.EmissiveMap, u_Material.EmissiveMap, unit++);
            UseMap(mtl.MetallicMap, u_Material.MetallicMap, unit++);
            UseMap(mtl.AlbedoMap, u_Material.AlbedoMap, unit++);
            UseMap(mtl.NormalMap, u_Material.NormalMap, unit++);

			// set properties
			glUniform3fv(u_Material.Emissive, 1, &mtl.Emissive.x);
            glUniform3fv(u_Material.Albedo, 1, &mtl.Albedo.x);
            glUniform1f(u_Material.Roughness, mtl.Roughness);
            glUniform1f(u_Material.Occlusion, mtl.Occlusion);
            glUniform1f(u_Material.Metallic, mtl.Metallic);
		}
        
    private:     
        uint32_t u_HasJoints = 0u;

        //-- light
        uint32_t u_NbrDirectLight = 0u;
        uint32_t u_NbrPointLight = 0u;
        uint32_t u_NbrSpotLight = 0u;        
        uint32_t u_LightSpace = 0u;
        // --
        MaterialUniform u_Material;
        //--
        uint32_t u_PrefilMap = 0u;
        uint32_t u_DepthMap = 0u;
        uint32_t u_IrradMap = 0u;
        uint32_t u_BrdfMap = 0u;
        //--
        uint32_t u_ViewPos = 0u;
        uint32_t u_Model = 0u;
        uint32_t u_View = 0u;
        uint32_t u_Proj = 0u;
    };   
}