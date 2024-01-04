#pragma once
#include "../Models/Model.h"
#include "../Textures/Texture.h"

namespace Empy
{
    // pbr material
    struct PbrMaterial 
    {
        EMPY_INLINE PbrMaterial(const PbrMaterial&) = default;
        EMPY_INLINE PbrMaterial() = default;

        glm::vec3 Albedo = glm::vec3(1.0f);
        float Roughness = 0.4f;
        float Metallic = 0.5f;

        Texture RoughnessMap;
        Texture MetallicMap;
        Texture NormalMap;
        Texture AlbedoMap;
    };

    // point light
    struct PointLight 
    {
        EMPY_INLINE PointLight(const PointLight&) = default;
        EMPY_INLINE PointLight() = default; 
        
        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 10.0f;
    };

    // point light
    struct SpotLight 
    {
        EMPY_INLINE SpotLight(const SpotLight&) = default;
        EMPY_INLINE SpotLight() = default; 
        
        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 3.5f;
		float FallOff = 60.5f;
        float CutOff = 20.0f;
    };

    // directional light
    struct DirectLight 
    {
        EMPY_INLINE DirectLight(const DirectLight&) = default;
        EMPY_INLINE DirectLight() = default; 
        
        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 10.0f;
    };

    // transform
    struct Transform3D
    {
        EMPY_INLINE Transform3D(const Transform3D&) = default;
        EMPY_INLINE Transform3D() = default; 

        EMPY_INLINE glm::mat4 Matrix() const 
        {
            return (glm::translate(glm::mat4(1.0f), Translate) * 
            glm::toMat4(glm::quat(glm::radians(Rotation))) * 
            glm::scale(glm::mat4(1.0f), Scale));
        }

        glm::vec3 Translate = glm::vec3(0.0f);  
        glm::vec3 Rotation = glm::vec3(0.0f);    
        glm::vec3 Scale = glm::vec3(1.0f);
    };

    // camera
    struct Camera3D
    {
        EMPY_INLINE Camera3D(const Camera3D&) = default;
        EMPY_INLINE Camera3D() = default;

        EMPY_INLINE glm::mat4 Frustum(const Transform3D& transform, float ratio) const 
        {
            return Projection(ratio) * View(transform);
        }

        EMPY_INLINE glm::mat4 View(const Transform3D& transform) const 
        {
            return glm::lookAt(transform.Translate, (transform.Translate + 
            glm::vec3(0.0f, 0.0f, -1.0f)), glm::vec3(0.0f, 1.0f, 0.0f)) * 
            glm::toMat4(glm::quat(glm::radians(transform.Rotation)));
        }

        EMPY_INLINE glm::mat4 Projection(float ratio) const 
        {
            return glm::perspective(FOV, ratio, NearPlane, FarPlane);
        }

        float NearPlane = 0.3000f;
        float FarPlane = 1000.0f;
        float FOV = 45.0f;
    };
}