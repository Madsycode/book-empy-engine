#pragma once
#include "Shader.h"

namespace Empy
{
    struct PbrShader : Shader 
    {        
        EMPY_INLINE PbrShader(const std::string& filename): Shader(filename) 
        {
            u_UseRoughnessMap = glGetUniformLocation(m_ShaderID, "u_material.UseRoughnessMap");
            u_UseOcclusionMap = glGetUniformLocation(m_ShaderID, "u_material.UseOcclusionMap");
            u_UseEmissiveMap = glGetUniformLocation(m_ShaderID, "u_material.UseEmissiveMap");
            u_UseMetallicMap = glGetUniformLocation(m_ShaderID, "u_material.UseMetallicMap");
            u_UseAlbedoMap = glGetUniformLocation(m_ShaderID, "u_material.UseAlbedoMap");
            u_UseNormalMap = glGetUniformLocation(m_ShaderID, "u_material.UseNormalMap");

            u_RoughnessMap = glGetUniformLocation(m_ShaderID, "u_material.RoughnessMap");
            u_OcclusionMap = glGetUniformLocation(m_ShaderID, "u_material.OcclusionMap");
            u_EmissiveMap = glGetUniformLocation(m_ShaderID, "u_material.EmissiveMap");
            u_MetallicMap = glGetUniformLocation(m_ShaderID, "u_material.MetallicMap");
            u_AlbedoMap = glGetUniformLocation(m_ShaderID, "u_material.AlbedoMap");
            u_NormalMap = glGetUniformLocation(m_ShaderID, "u_material.NormalMap");

            u_Roughness = glGetUniformLocation(m_ShaderID, "u_material.Roughness");
            u_Occlusion = glGetUniformLocation(m_ShaderID, "u_material.Occlusion");
            u_Emissive = glGetUniformLocation(m_ShaderID, "u_material.Emissive");
            u_Metallic = glGetUniformLocation(m_ShaderID, "u_material.Metallic");
            u_Albedo = glGetUniformLocation(m_ShaderID, "u_material.Albedo");

            u_NbrDirectLight = glGetUniformLocation(m_ShaderID, "u_nbrDirectLight");
            u_NbrPointLight = glGetUniformLocation(m_ShaderID, "u_nbrPointLight");
            u_NbrSpotLight = glGetUniformLocation(m_ShaderID, "u_nbrSpotLight");

            u_PrefilMap = glGetUniformLocation(m_ShaderID, "u_prefilMap");            
            u_IrradMap = glGetUniformLocation(m_ShaderID, "u_irradMap");
            u_BrdfMap = glGetUniformLocation(m_ShaderID, "u_brdfMap");

            u_LightSpace = glGetUniformLocation(m_ShaderID, "u_lightSpace");
            u_DepthMap = glGetUniformLocation(m_ShaderID, "u_depthMap");

            u_ViewPos = glGetUniformLocation(m_ShaderID, "u_viewPos");
            u_Model = glGetUniformLocation(m_ShaderID, "u_model");
            u_View = glGetUniformLocation(m_ShaderID, "u_view");
            u_Proj = glGetUniformLocation(m_ShaderID, "u_proj");
            
            u_HasJoints = glGetUniformLocation(m_ShaderID, "u_hasJoints");
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

        EMPY_INLINE void Draw(Model3D& model, PbrMaterial& material, Transform3D& transform)
        {
            glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));   
            glUniform1i(u_HasJoints, model->HasJoint()); 

            glUniform3fv(u_Emissive, 1, &material.Emissive.x);
            glUniform3fv(u_Albedo, 1, &material.Albedo.x);
            glUniform1f(u_Roughness, material.Roughness);
            glUniform1f(u_Occlusion, material.Occlusion);
            glUniform1f(u_Metallic, material.Metallic);

            int32_t unit = 4; // <-- starts at unit 4
            bool useMap = false;

            // albedo map
            useMap = material.AlbedoMap != nullptr;
            glUniform1i(u_UseAlbedoMap, useMap);
            if(useMap) { material.AlbedoMap->Use(u_AlbedoMap, unit++); }
            
            // normal map
            useMap = material.NormalMap != nullptr;
            glUniform1i(u_UseNormalMap, useMap);
            if(useMap) { material.NormalMap->Use(u_NormalMap, unit++); }
           
            // mettalic map
            useMap = material.MetallicMap != nullptr;
            glUniform1i(u_UseMetallicMap, useMap);
            if(useMap) { material.MetallicMap->Use(u_MetallicMap, unit++); }
          
            // roughness map
            useMap = material.RoughnessMap != nullptr;
            glUniform1i(u_UseRoughnessMap, useMap);
            if(useMap) { material.RoughnessMap->Use(u_RoughnessMap, unit++); }

            // emissive map
            useMap = material.EmissiveMap != nullptr;
            glUniform1i(u_UseEmissiveMap, useMap);
            if(useMap) { material.EmissiveMap->Use(u_EmissiveMap, unit++); }

            // occlusion map
            useMap = material.OcclusionMap != nullptr;
            glUniform1i(u_UseOcclusionMap, useMap);
            if(useMap) { material.OcclusionMap->Use(u_OcclusionMap, unit++); }

            // render model
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
       
        EMPY_INLINE void SetJoints(std::vector<glm::mat4>& transforms) 
        {
            for (size_t i = 0; i < transforms.size() && i < 100; ++i) 
            {
                std::string uniform = "u_joints[" + std::to_string(i) + "]";
                uint32_t u_joint = glGetUniformLocation(m_ShaderID, uniform.c_str());
                glUniformMatrix4fv(u_joint, 1, GL_FALSE, glm::value_ptr(transforms[i]));
            }
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
        uint32_t u_HasJoints = 0u;

        //-- light
        uint32_t u_NbrDirectLight = 0u;
        uint32_t u_NbrPointLight = 0u;
        uint32_t u_NbrSpotLight = 0u;        
        // --
        uint32_t u_UseRoughnessMap = 0u;
        uint32_t u_UseOcclusionMap = 0u;
        uint32_t u_UseEmissiveMap = 0u;
        uint32_t u_UseMetallicMap = 0u;
        uint32_t u_UseAlbedoMap = 0u;
        uint32_t u_UseNormalMap = 0u;
        // --
        uint32_t u_RoughnessMap = 0u; 
        uint32_t u_OcclusionMap = 0u;  
        uint32_t u_EmissiveMap = 0u;  
        uint32_t u_MetallicMap = 0u;  
        uint32_t u_AlbedoMap = 0u;    
        uint32_t u_NormalMap = 0u;    
        // --
        uint32_t u_Roughness = 0u;
        uint32_t u_Occlusion = 0u;
        uint32_t u_Emissive = 0u;
        uint32_t u_Metallic = 0u;
        uint32_t u_Albedo = 0u;
        //--
        uint32_t u_PrefilMap = 0u;
        uint32_t u_IrradMap = 0u;
        uint32_t u_BrdfMap = 0u;
        //--
        uint32_t u_LightSpace = 0u;
        uint32_t u_DepthMap = 0u;
        //--
        uint32_t u_ViewPos = 0u;
        uint32_t u_Model = 0u;
        uint32_t u_View = 0u;
        uint32_t u_Proj = 0u;
    };   
}