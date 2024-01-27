#pragma once
#include "ECS.h"
#include "Common/YAML.h"

namespace Empy
{
    struct DataSerializer
    {
        EMPY_INLINE void Serialize(EntityRegistry& scene, const std::string& path)
        {
            YAML::Emitter emitter;

            emitter << YAML::BeginMap;
            {
                emitter << YAML::Key << "ENTITIES" << YAML::Value << YAML::BeginSeq;
                {
                    scene.each([&] (EntityID entt) 
                    {   
                        // create entity object
                        Entity entity(&scene, entt);

                        // serialize component
                        emitter << YAML::BeginMap;       
                        {
                            // serialize transform component
                            if (entity.template Has<InfoComponent>()) 
                            {
                                auto& info = entity.template Get<InfoComponent>();
                                emitter << YAML::Key << "InfoComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "UID" << YAML::Value << info.UID;
                                    emitter << YAML::Key << "Name" << YAML::Value << info.Name;
                                    emitter << YAML::Key << "Parent" << YAML::Value << info.Parent;
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize transform component
                            if (entity.template Has<CameraComponent>()) 
                            {
                                auto& camera = entity.template Get<CameraComponent>().Camera;
                                emitter << YAML::Key << "CameraComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "NearPlane" << YAML::Value << camera.NearPlane;
                                    emitter << YAML::Key << "FarPlane" << YAML::Value << camera.FarPlane;
                                    emitter << YAML::Key << "FOV" << YAML::Value << camera.FOV;
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize transform component
                            if (entity.template Has<TransformComponent>()) 
                            {
                                auto& transform = entity.template Get<TransformComponent>().Transform;
                                emitter << YAML::Key << "TransformComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Translate" << YAML::Value << transform.Translate;
                                    emitter << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
                                    emitter << YAML::Key << "Scale" << YAML::Value << transform.Scale;
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize rigidbody compoment
                            if (entity.template Has<RigidBodyComponent>()) 
                            {
                                auto& body = entity.template Get<RigidBodyComponent>().RigidBody;
                                emitter << YAML::Key << "RigidBodyComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Density" << YAML::Value << body.Density;
                                    emitter << YAML::Key << "Dynamic" << YAML::Value << body.Dynamic;
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize collider component
                            if (entity.template Has<ColliderComponent>()) 
                            {
                                auto& collider = entity.template Get<ColliderComponent>().Collider;
                                std::string type(magic_enum::enum_name(collider.Type));

                                emitter << YAML::Key << "ColliderComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "DynamicFriction" << YAML::Value << collider.DynamicFriction;
                                    emitter << YAML::Key << "StaticFriction" << YAML::Value << collider.StaticFriction;
                                    emitter << YAML::Key << "Restitution" << YAML::Value << collider.Restitution;
                                    emitter << YAML::Key << "Type" << YAML::Value << type;
                                }
                                emitter << YAML::EndMap;
                            }
                           
                            // serialize pointlight component
                            if (entity.template Has<DirectLightComponent>()) 
                            {
                                auto& light = entity.template Get<DirectLightComponent>().Light;

                                emitter << YAML::Key << "DirectLightComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
                                    emitter << YAML::Key << "Radiance" << YAML::Value << light.Radiance;
                                    emitter << YAML::Key << "Bias" << YAML::Value << light.ShadowBias;
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize pointlight component
                            if (entity.template Has<PointLightComponent>()) 
                            {
                                auto& light = entity.template Get<PointLightComponent>().Light;

                                emitter << YAML::Key << "PointLightComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
                                    emitter << YAML::Key << "Radiance" << YAML::Value << light.Radiance;
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize spotlight component
                            if (entity.template Has<SpotLightComponent>()) 
                            {
                                auto& light = entity.template Get<SpotLightComponent>().Light;

                                emitter << YAML::Key << "SpotLightComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
                                    emitter << YAML::Key << "Radiance" << YAML::Value << light.Radiance;
                                    emitter << YAML::Key << "FallOff" << YAML::Value << light.FallOff;
                                    emitter << YAML::Key << "CutOff" << YAML::Value << light.CutOff;
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize script component
                            if (entity.template Has<ScriptComponent>()) 
                            {
                                emitter << YAML::Key << "ScriptComponent" << YAML::BeginMap;
                                {                                    
                                    auto& asset = entity.template Get<ScriptComponent>().Script;
                                    emitter << YAML::Key << "Script" << YAML::Value << asset;                                    
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize script component
                            if (entity.template Has<SkyboxComponent>()) 
                            {
                                emitter << YAML::Key << "SkyboxComponent" << YAML::BeginMap;
                                {
                                    auto& asset = entity.template Get<SkyboxComponent>().Skybox;                                    
                                    emitter << YAML::Key << "Skybox" << YAML::Value << asset;                                    
                                }
                                emitter << YAML::EndMap;
                            }

                            // serialize model component
                            if (entity.template Has<ModelComponent>()) 
                            {
                                auto& comp = entity.template Get<ModelComponent>();               
                                emitter << YAML::Key << "ModelComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Material" << YAML::Value << comp.Material;                                    
                                    emitter << YAML::Key << "Model" << YAML::Value << comp.Model;                                                                    
                                }
                                emitter << YAML::EndMap;
                            }
                        }  
                        emitter << YAML::EndMap;
                    });
                }
                emitter << YAML::EndSeq;
            }
            emitter << YAML::EndMap;

            std::ofstream filepath(path);
            filepath << emitter.c_str();
        }

        EMPY_INLINE void Serialize(AssetRegistry& registry, const std::string& path)
        {
            YAML::Emitter emitter;

            emitter << YAML::BeginMap;
            {
                emitter << YAML::Key << "ASSETS" << YAML::Value << YAML::BeginSeq;
                {
                    registry.View([&] (Asset* asset) 
                    {
                        // asset type to string
                        std::string typeName(magic_enum::enum_name(asset->Type));

                        // serialize asset item
                        emitter << YAML::BeginMap;      
                        {
                            // asset auxiliaries
                            emitter << YAML::Key << "UID" << YAML::Value << asset->UID;
                            emitter << YAML::Key << "Type" << YAML::Value << typeName;
                            emitter << YAML::Key << "Name" << YAML::Value << asset->Name;
                            emitter << YAML::Key << "Source" << YAML::Value << asset->Source;

                            // serialize material
                            if(asset->Type == AssetType::MATERIAL)
                            {
                                auto mtl = static_cast<MaterialAsset*>(asset);
                                emitter << YAML::Key << "Properties" << YAML::BeginMap;
                                {
                                    // texture assets
                                    emitter << YAML::Key << "RoughnessMap" << YAML::Value << mtl->RoughnessMap;
                                    emitter << YAML::Key << "OcclusionMap" << YAML::Value << mtl->OcclusionMap;
                                    emitter << YAML::Key << "EmissiveMap" << YAML::Value << mtl->EmissiveMap;
                                    emitter << YAML::Key << "MetallicMap" << YAML::Value << mtl->MetallicMap;
                                    emitter << YAML::Key << "AlbedoMap" << YAML::Value << mtl->AlbedoMap;
                                    emitter << YAML::Key << "NormalMap" << YAML::Value << mtl->NormalMap;

                                    // properties
                                    emitter << YAML::Key << "Roughness" << YAML::Value << mtl->Data.Roughness;
                                    emitter << YAML::Key << "Occlusion" << YAML::Value << mtl->Data.Occlusion;
                                    emitter << YAML::Key << "Metallic" << YAML::Value << mtl->Data.Metallic;
                                    emitter << YAML::Key << "Emissive" << YAML::Value << mtl->Data.Emissive;
                                    emitter << YAML::Key << "Albedo" << YAML::Value << mtl->Data.Albedo;
                                }
                                emitter << YAML::EndMap;
                            }                            
                            // serialize texture
                            else if(asset->Type == AssetType::TEXTURE)
                            {
                                auto texture = static_cast<TextureAsset*>(asset);
                                emitter << YAML::Key << "Properties" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "IsHDR" << YAML::Value << texture->IsHDR;
                                    emitter << YAML::Key << "FlipV" << YAML::Value << texture->FlipV;
                                }
                                emitter << YAML::EndMap;
                            }
                            // serialize skybox
                            else if(asset->Type == AssetType::SKYBOX)
                            {
                                auto skybox = static_cast<SkyboxAsset*>(asset);
                                emitter << YAML::Key << "Properties" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "IsHDR" << YAML::Value << skybox->IsHDR;
                                    emitter << YAML::Key << "FlipV" << YAML::Value << skybox->FlipV;
                                    emitter << YAML::Key << "Size" << YAML::Value << skybox->Size;
                                }
                                emitter << YAML::EndMap;
                            }
                            // serialize model
                            else if(asset->Type == AssetType::MODEL)
                            {
                                auto model = static_cast<ModelAsset*>(asset);
                                emitter << YAML::Key << "Properties" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "HasJoints" << YAML::Value << model->HasJoints;
                                }
                                emitter << YAML::EndMap;
                            }                           
                        }  
                        emitter << YAML::EndMap;
                    });
                }
                emitter << YAML::EndSeq;
            }
            emitter << YAML::EndMap;

            std::ofstream filepath(path);
            filepath << emitter.c_str();
        }

        EMPY_INLINE void Deserialize(EntityRegistry& scene, const std::string& path)
        {
            try 
            {
                auto root = YAML::LoadFile(path);
                auto& nodes = root["ENTITIES"];
                scene.clear();

                // deserialize nodes
                for(auto& node : nodes)
                {
                    // create entity
                    EntityID entity = scene.create();

                    // deserialize entt infos
                    if (auto& data = node["InfoComponent"]) 
                    {
                        auto& info = scene.emplace<InfoComponent>(entity);
                        info.Parent = data["Parent"].as<AssetID>();
                        info.Name = data["Name"].as<std::string>();
                        info.UID = data["UID"].as<AssetID>();
                    }

                    // deserialize entt infos
                    if (auto& data = node["CameraComponent"]) 
                    {
                        auto& camera = scene.emplace<CameraComponent>(entity).Camera;
                        camera.NearPlane = data["NearPlane"].as<float>();
                        camera.FarPlane = data["FarPlane"].as<float>();
                        camera.FOV = data["FOV"].as<float>();
                    }

                    // deserialize transform
                    if (auto& data = node["TransformComponent"]) 
                    {
                        auto& transform = scene.emplace<TransformComponent>(entity).Transform;
                        transform.Translate = data["Translate"].as<glm::vec3>();
                        transform.Rotation = data["Rotation"].as<glm::vec3>();
                        transform.Scale = data["Scale"].as<glm::vec3>();
                    }

                    // deserialize rigidbody
                    if (auto& data = node["RigidBodyComponent"]) 
                    {
                        auto& body = scene.emplace<RigidBodyComponent>(entity).RigidBody;
                        body.Density = data["Density"].as<float>();
                        body.Dynamic = data["Dynamic"].as<bool>();
                    }

                    // deserialize collider
                    if (auto& data = node["ColliderComponent"]) 
                    {
                        auto& collider = scene.emplace<ColliderComponent>(entity).Collider;
                        collider.DynamicFriction = data["DynamicFriction"].as<float>();
                        collider.StaticFriction = data["StaticFriction"].as<float>();
                        collider.Restitution = data["Restitution"].as<float>();
                        // get type
                        const auto name = data["Type"].as<std::string>();
                        auto type = magic_enum::enum_cast<ColliderType>(name);
                        if(type.has_value()) { collider.Type = type.value(); }
                    }

                    // deserialize model
                    if (auto& data = node["ModelComponent"]) 
                    {
                        auto& comp = scene.emplace<ModelComponent>(entity);
                        comp.Material = data["Material"].as<AssetID>();
                        comp.Model = data["Model"].as<AssetID>();
                    }

                    // deserialize script
                    if (auto& data = node["ScriptComponent"]) 
                    {
                        auto& comp = scene.emplace<ScriptComponent>(entity);
                        comp.Script = data["Script"].as<AssetID>();
                    }

                    // deserialize script
                    if (auto& data = node["SkyboxComponent"]) 
                    {
                        auto& comp = scene.emplace<SkyboxComponent>(entity);
                        comp.Skybox = data["Skybox"].as<AssetID>();
                    }

                    // deserialize direct light
                    if (auto& data = node["DirectLightComponent"]) 
                    {
                        auto& light = scene.emplace<DirectLightComponent>(entity).Light;
                        light.Radiance = data["Radiance"].as<glm::vec3>();
                        light.Intensity = data["Intensity"].as<float>();
                        light.ShadowBias = data["Bias"].as<float>();
                    }

                    // deserialize point light
                    if (auto& data = node["PointLightComponent"]) 
                    {
                        auto& light = scene.emplace<PointLightComponent>(entity).Light;
                        light.Radiance = data["Radiance"].as<glm::vec3>();
                        light.Intensity = data["Intensity"].as<float>();
                    }

                    // deserialize point light
                    if (auto& data = node["SpotLightComponent"]) 
                    {
                        auto& light = scene.emplace<SpotLightComponent>(entity).Light;
                        light.Radiance = data["Radiance"].as<glm::vec3>();
                        light.Intensity = data["Intensity"].as<float>();
                        light.FallOff = data["FallOff"].as<float>();
                        light.CutOff = data["CutOff"].as<float>();
                    }
                }
            }
            catch (YAML::ParserException& e) 
            {
                EMPY_ERROR("failed to deserialize scene!");
            }  
        }

        EMPY_INLINE void Deserialize(AssetRegistry& registry, const std::string& path)
        {
            try 
            {
                auto root = YAML::LoadFile(path);
                auto& nodes = root["ASSETS"];

                // deserialize nodes
                for(auto& node : nodes)
                {
                    Asset* asset = nullptr;
                    auto props = node["Properties"]; 
                    auto uid = node["UID"].as<AssetID>();
                    auto name = node["Name"].as<std::string>();
                    auto source = node["Source"].as<std::string>();

                    // get asset type
                    auto typeName = node["Type"].as<std::string>();
                    auto opt = magic_enum::enum_cast<AssetType>(typeName);
                    auto type = (opt.has_value()) ? opt.value() : AssetType::UNKNOWN;                    

                    // create instance
                    if(type == AssetType::MATERIAL && props) 
                    { 
                        auto mtlAsset = registry.AddMaterial(uid, source);

                        // set material textures
                        mtlAsset->RoughnessMap = props["RoughnessMap"].as<AssetID>();
                        mtlAsset->OcclusionMap = props["OcclusionMap"].as<AssetID>();
                        mtlAsset->EmissiveMap = props["EmissiveMap"].as<AssetID>();
                        mtlAsset->MetallicMap = props["MetallicMap"].as<AssetID>();
                        mtlAsset->AlbedoMap = props["AlbedoMap"].as<AssetID>();
                        mtlAsset->NormalMap = props["NormalMap"].as<AssetID>();

                        // set material props
                        mtlAsset->Data.Emissive = props["Emissive"].as<glm::vec3>();
                        mtlAsset->Data.Occlusion = props["Occlusion"].as<float>();
                        mtlAsset->Data.Roughness = props["Roughness"].as<float>();
                        mtlAsset->Data.Metallic = props["Metallic"].as<float>();
                        mtlAsset->Data.Albedo = props["Albedo"].as<glm::vec3>();

                        // cast asset
                        asset = (Asset*)mtlAsset.get();
                    }
                    else if(type == AssetType::TEXTURE && props) 
                    { 
                        // set properties
                        bool isHDR = props["IsHDR"].as<bool>();
                        bool flipV = props["FlipV"].as<bool>();
                        asset = (Asset*)registry.AddTexture(uid, source, isHDR, flipV).get();                     
                    }
                    else if(type == AssetType::SKYBOX && props) 
                    { 
                        // set properties
                        bool isHDR = props["IsHDR"].as<bool>();
                        bool flipV = props["FlipV"].as<bool>();
                        auto size = props["Size"].as<int32_t>();
                        asset = (Asset*)registry.AddSkybox(uid, source, size, isHDR, flipV).get();    
                    }
                    else if(type == AssetType::MODEL && props) 
                    { 
                        bool hasJoints = props["HasJoints"].as<bool>();
                        asset = (Asset*)registry.AddModel(uid, source, hasJoints).get();    
                    }
                    else if(type == AssetType::SCRIPT) 
                    { 
                        asset = (Asset*)registry.AddScript(uid, source).get();
                    }
                    else if(type == AssetType::SCENE) 
                    { 
                        asset = (Asset*)registry.AddScene(uid, source).get();
                    }
                    else 
                    {
                        EMPY_ERROR("failed to deserialize asset: invalid type");
                        return;
                    }

                    // set asset properties
                    asset->Source = source;
                    asset->Name = name;
                }
            }
            catch (YAML::ParserException& e) 
            {
                EMPY_ERROR("failed to deserialize assets!");
            }  
        }
    };
}