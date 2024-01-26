#pragma once
#include "Window/Window.h"
#include "Auxiliaries/ECS.h"

namespace Empy
{
    struct ScriptContext
    {
        EMPY_INLINE ScriptContext(EntityRegistry* scene, AppWindow* window)
        {       
            // import lua libraries
            m_Lua.open_libraries(sol::lib::base);

            // load empy api module
            m_Lua.require_file("Inputs", "Resources/Scripts/EmpyEngine/Input.lua");   
            m_Lua.require_file("Core", "Resources/Scripts/EmpyEngine/Core.lua");   

            // runtime type identifiers
            m_Lua["TRANSFORM"] = TypeID<TransformComponent>();

            // add transform data type
            m_Lua.new_usertype<Transform3D>("Transform3D",
                "Translate", &Transform3D::Translate,
                "Rotation", &Transform3D::Rotation,
                "Scale", &Transform3D::Scale
            );

            // add vec3 data type
            m_Lua.new_usertype<glm::vec3>("Vec3",
                sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),   
                "x", &glm::vec3::x,
                "y", &glm::vec3::y,
                "z", &glm::vec3::z
            ); 

            // register window inputs callbacks      
            SetApiFunctions(scene, window);       
        }
        
        // creates instance of existing script
        EMPY_INLINE bool AttachScript(Entity& entity, const std::string& name)
        {                    
            // check if handle is correct and has a constructor
            if (!m_Lua[name].valid() && !m_Lua[name]["Constructor"].valid()) 
            {
                EMPY_ERROR("failed to create script: invalid script name!"); 
                return false;
            }

            // create instance script 
            auto object = m_Lua[name]["Constructor"](entity.ID());

            // check if obj is valid
            if(!object.valid())
            {
                // handle failure
                sol::error error = object;
                sol::call_status status = object.status();
                EMPY_ERROR("failed to create script: {}", error.what());
                return false;
            }

            auto& instance = entity.Get<ScriptComponent>().Instance;
            instance = std::make_unique<Script>(object, name);
            instance->OnStart();
            return true; 
        }        

        // loads script into lua state
        EMPY_INLINE std::string LoadScript(const std::string& filepath)
        {
            std::filesystem::path path(filepath);
            auto scriptName = path.stem().string();

            // check if already loaded
            if(m_Lua[scriptName].valid())
            {
                return scriptName;
            }

            // check if modules are loaded
            if(!m_Lua["Initializer"].valid()) 
            {
                EMPY_ERROR("failed to load script! core not initialized!");
                return "";
            }

            // check if script file exits
            if(!std::filesystem::exists(path))
            {
                EMPY_ERROR("failed to load script: invalid file path!");
                return "";
            }

            // create script class handle
            m_Lua[scriptName] = m_Lua["Initializer"]();
            m_Lua.script_file(filepath);   
            return scriptName;
        }

    private:
        // registers functions to interact with entity
        EMPY_INLINE void SetApiFunctions(EntityRegistry* scene, AppWindow* window)
        {
            // api function to get entity transform
            m_Lua.set_function("ApiGetTransform", [this, scene] (EntityID entity)
            {
                return std::ref(scene->get<TransformComponent>(entity).Transform);
            });

            // api function to check if mouse down
            m_Lua.set_function("ApiMouseDown", [this, window] (int32_t button)
            {
                return window->IsMouse(button);
            });

            // api function to apply force on actor
            m_Lua.set_function("ApiApplyForce", [this, scene] 
            (EntityID entity, const glm::vec3& force)
            {
                // return reference to transform
                if(scene->all_of<RigidBodyComponent>(entity))
                {
                    auto& body = scene->get<RigidBodyComponent>(entity).RigidBody;
                    if(body.Dynamic)
                    {
                        auto actor = static_cast<PxRigidDynamic*>(body.Actor);
                        actor->addForce(ToPxVec3(force), 
                        PxForceMode::eACCELERATION);                
                        return true;
                    }          
                    EMPY_ERROR("trying to apply force to static body!");          
                    return false;
                }
                EMPY_ERROR("trying to apply force to non physics entity!");          
                return false;
            });

            // api function to check if key pressed
            m_Lua.set_function("ApiKeyDown", [this, window] (int32_t key)
            {
                return window->IsKey(key);
            });

            // api function to destroy entity
            m_Lua.set_function("ApiDestroy", [this, scene] (EntityID entity)
            {
                // return if entity is dead!
                if(scene->valid(entity) == false) { return; }
                               
                // entity has rigidbody component                
                if(scene->all_of<RigidBodyComponent>(entity)) 
                {
                    // destroy actor user data      
                    auto& body = scene->get<RigidBodyComponent>(entity).RigidBody;     
                    auto userData = static_cast<EntityID*>(body.Actor->userData);
                    body.Actor->release();
                    EMPY_DELETE(userData);
                } 

                // destroy collider material & shape
                if(scene->all_of<ColliderComponent>(entity)) 
                {
                    // destroy actor user data      
                    auto& collider = scene->get<ColliderComponent>(entity).Collider;     
                    collider.Material->release();
                    collider.Shape->release();
                }                

                // entity has script component
                if(scene->all_of<ScriptComponent>(entity)) 
                {
                    // call on destroy function
                    scene->get<ScriptComponent>(entity).Instance->OnDestroy();     
                }

                // destroy entity from scene
                scene->destroy(entity);
            });
        }

    private:
        sol::state m_Lua;
    };
}