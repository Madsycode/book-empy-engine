#pragma once
#include "../Models/Model.h"

namespace Empy
{
    // material
   struct Material3D 
   {
        EMPY_INLINE Material3D(const Material3D&) = default;
        EMPY_INLINE Material3D() = default; 

        glm::vec3 Specular = glm::vec3(0.8f, 0.5f, 0.6f);
        glm::vec3 Diffuse = glm::vec3(1.0f, 0.5f, 0.3f);
        glm::vec3 Ambient = glm::vec3(0.03f);
        float Shininess = 1.0f;
    };

    // point light
    struct PointLight 
    {
        EMPY_INLINE PointLight(const PointLight&) = default;
        EMPY_INLINE PointLight() = default; 
        
        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 1.0f;
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
            return glm::lookAt(transform.Translate, (transform.Translate + glm::vec3(0, 0, -1)), 
            glm::vec3(0, 1, 0)) * glm::toMat4(glm::quat(glm::radians(transform.Rotation)));
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