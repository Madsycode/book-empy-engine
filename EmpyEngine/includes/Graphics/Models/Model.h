#pragma once
#include <assimp/postprocess.h>
#include <assimp/quaternion.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Animator.h"

namespace Empy
{
	// abstract model
	struct Model 
	{
		EMPY_INLINE virtual bool HasJoint() { return false; }
		EMPY_INLINE virtual void Load(const std::string&) {}
		EMPY_INLINE virtual void Draw(uint32_t mode) {}
	};	

	// static model
    struct StaticModel : Model
    {
        EMPY_INLINE StaticModel() = default;

        EMPY_INLINE StaticModel(const std::string& path)
        {
            Load(path);
        }

        EMPY_INLINE void Load(const std::string& path) override final
        {
			uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure |
			aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals | 
			aiProcess_GenUVCoords | aiProcess_FlipUVs;

            Assimp::Importer importer;
			const aiScene* ai_scene = importer.ReadFile(path, flags);

			if (!ai_scene || ai_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
				EMPY_ERROR("failed to load model-{}", importer.GetErrorString());
				return;
			}

            // parse all meshes
			ParseNode(ai_scene, ai_scene->mRootNode);
        }

        EMPY_INLINE void Draw(uint32_t mode) override final
        {
            for(auto& mesh : m_Meshes)
            {
                mesh->Draw(mode);
            }
        }

    private:
        EMPY_INLINE void ParseNode(const aiScene* ai_scene, aiNode* ai_node) 
        {
			for (uint32_t i = 0; i < ai_node->mNumMeshes; i++) 
            {
				ParseMesh(ai_scene->mMeshes[ai_node->mMeshes[i]]);
			}

			for (uint32_t i = 0; i < ai_node->mNumChildren; i++) 
            {
				ParseNode(ai_scene, ai_node->mChildren[i]);
			}
		}

        EMPY_INLINE void ParseMesh(aiMesh* ai_mesh) 
        {
			// mesh data
			MeshData<ShadedVertex> data;

			// vertices
			for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++) 
            {
				ShadedVertex vertex;
				// positions
				vertex.Position = AssimpToVec3(ai_mesh->mVertices[i]);
				// normals
				vertex.Normal = AssimpToVec3(ai_mesh->mNormals[i]);

				// texcoords
				vertex.UVs.x = ai_mesh->mTextureCoords[0][i].x;
				vertex.UVs.y = ai_mesh->mTextureCoords[0][i].y;

				// bi-tangent
				vertex.Bitangent = glm::normalize(AssimpToVec3(ai_mesh->mBitangents[i])); 
				vertex.Tangent = glm::normalize(AssimpToVec3(ai_mesh->mTangents[i])); 
				
				// push vertex
				data.Vertices.push_back(vertex);
			}

			// indices	
			for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++) 
            {
				for (uint32_t k = 0; k < ai_mesh->mFaces[i].mNumIndices; k++) 
                {
					data.Indices.push_back(ai_mesh->mFaces[i].mIndices[k]);
				}
			}

            // create new mesh instance
			m_Meshes.push_back(std::make_unique<ShadedMesh>(data));
		}

    private:
        std::vector<std::unique_ptr<ShadedMesh>> m_Meshes;
    };

	// animated model
	struct SkeletalModel : Model
	{
    	using JointMap = std::unordered_map<std::string, Joint>;

		EMPY_INLINE SkeletalModel(const std::string& path)
		{
			Load(path);
		}

		EMPY_INLINE SkeletalModel() = default;		

		EMPY_INLINE bool HasJoint() override final { return m_JointCount; }

		EMPY_INLINE void Load(const std::string& path) override final
        {
			uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
				aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure |
				aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals | 
				aiProcess_SortByPType | aiProcess_JoinIdenticalVertices | 
				aiProcess_FlipUVs | aiProcess_GenUVCoords | 
				aiProcess_LimitBoneWeights; 
			
            Assimp::Importer importer;
			const aiScene* ai_scene = importer.ReadFile(path, flags);
			if (!ai_scene || ai_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) 
			{
				EMPY_ERROR("failed to load model: '{}'", importer.GetErrorString());
				return;
			}

			m_Animator = std::make_shared<Animator>();
			m_Animator->m_GlobalTransform = glm::inverse(AssimpToMat4(ai_scene->mRootNode->mTransformation));

			// temp joints map
			JointMap jointMap = {};

            // parse all meshes
			ParseNode(ai_scene, ai_scene->mRootNode, jointMap);

			// parse animations
			ParseAnimations(ai_scene, jointMap);
		}
		
		EMPY_INLINE void Draw(uint32_t mode) override final
        {
            for(auto& mesh : m_Meshes)
            {
                mesh->Draw(mode);
            }
        }

		EMPY_INLINE auto GetAnimator()
		{
			return m_Animator;
		}

	private:
		EMPY_INLINE void ParseNode(const aiScene* ai_scene, aiNode* ai_node, JointMap& jointMap) 
        {
			for (uint32_t i = 0; i < ai_node->mNumMeshes; i++) 
            {
				ParseMesh(ai_scene->mMeshes[ai_node->mMeshes[i]], jointMap);
			}

			for (uint32_t i = 0; i < ai_node->mNumChildren; i++) 
            {
				ParseNode(ai_scene, ai_node->mChildren[i], jointMap);
			}
		}

		EMPY_INLINE void SetVertexJoint(SkeletalVertex& vertex, int32_t id, float weight) 
		{
			for (uint32_t i = 0; i < 4; i++) 
			{
				if (vertex.Joints[i] < 0) 
				{
					vertex.Weights[i] = weight;
					vertex.Joints[i] = id;
					return;
				}
			}
		}
		
		EMPY_INLINE void ParseHierarchy(aiNode* ai_node, Joint& joint, JointMap& jointMap)
		{
			std::string jointName(ai_node->mName.C_Str());

			if(jointMap.count(jointName))
			{
				joint = jointMap[jointName];

				for (uint32_t i = 0; i < ai_node->mNumChildren; i++) 
				{
					Joint child;
					ParseHierarchy(ai_node->mChildren[i], child, jointMap);
					joint.Children.push_back(std::move(child));
				}
			}
			else
			{
				for (uint32_t i = 0; i < ai_node->mNumChildren; i++) 
				{
					ParseHierarchy(ai_node->mChildren[i], joint, jointMap);
				}
			}
		}
		
		EMPY_INLINE void ParseAnimations(const aiScene* ai_scene, JointMap& jointMap) 
        {
			// parse animation data
			for (uint32_t i = 0; i < ai_scene->mNumAnimations; i++) 
			{
				auto ai_anim = ai_scene->mAnimations[i];

				Animation animation;
				animation.Name = ai_anim->mName.C_Str();
				animation.Duration = ai_anim->mDuration;
				animation.Speed = ai_anim->mTicksPerSecond;
				m_Animator->m_Animations.push_back(animation);

				// parse animation keys
				for (uint32_t j = 0; j < ai_anim->mNumChannels; j++) 
				{
					aiNodeAnim* ai_channel = ai_anim->mChannels[j];
					auto jointName(ai_channel->mNodeName.C_Str());
					if(!jointMap.count(jointName)) { continue; }

					for (uint32_t k = 0; k < ai_channel->mNumPositionKeys; k++) 
					{
						KeyFrame key;
						key.Position = AssimpToVec3(ai_channel->mPositionKeys[k].mValue);
						key.Rotation = AssimpToQuat(ai_channel->mRotationKeys[k].mValue);
						key.Scale = AssimpToVec3(ai_channel->mScalingKeys[k].mValue);
						key.TimeStamp = ai_channel->mPositionKeys[k].mTime;
						jointMap[jointName].Keys.push_back(key);
					}					
				}		
			}

			// parse jointMap hierarchy
			ParseHierarchy(ai_scene->mRootNode, m_Animator->m_Root, jointMap);

			// initialize animator
			m_Animator->m_Transforms.resize(m_JointCount);
		}
				
		EMPY_INLINE void ParseMesh(const aiMesh* ai_mesh, JointMap& jointMap) 
        {
			// mesh data
			MeshData<SkeletalVertex> data;

			// vertices
			for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++) 
            {
				SkeletalVertex vertex;
				// positions
				vertex.Position = AssimpToVec3(ai_mesh->mVertices[i]);
				// normals
				vertex.Normal = AssimpToVec3(ai_mesh->mNormals[i]);
				// texcoords
				vertex.UVs.x = ai_mesh->mTextureCoords[0][i].x;
				vertex.UVs.y = ai_mesh->mTextureCoords[0][i].y;
				// push vertex
				data.Vertices.push_back(std::move(vertex));
			}

			// indices	
			for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++) 
            {
				for (uint32_t k = 0; k < ai_mesh->mFaces[i].mNumIndices; k++) 
                {
					data.Indices.push_back(ai_mesh->mFaces[i].mIndices[k]);
				}
			}

			// joints
			for (uint32_t i = 0; i < ai_mesh->mNumBones; i++) 
			{
				aiBone* ai_bone = ai_mesh->mBones[i];

				// get joint name
				std::string jointName(ai_bone->mName.C_Str());

				// add joint if not found
				if(jointMap.count(jointName) == 0)
				{	
					jointMap[jointName].Offset = AssimpToMat4(ai_bone->mOffsetMatrix);
					jointMap[jointName].Index = m_JointCount++;
					jointMap[jointName].Name = jointName;
				}

				// set vertex joint weights
				for (uint32_t j = 0; j < ai_bone->mNumWeights; j++) 
				{
					SetVertexJoint(data.Vertices[ai_bone->mWeights[j].mVertexId], 
					jointMap[jointName].Index, ai_bone->mWeights[j].mWeight);
				}
			}

            // create new mesh instance
			m_Meshes.push_back(std::make_unique<SkeletalMesh>(data));
		}
		
	private:
	 	std::vector<std::unique_ptr<SkeletalMesh>> m_Meshes;
		std::shared_ptr<Animator> m_Animator;
		uint32_t m_JointCount = 0;		
	};

	// model typedef
	using Animator3D = std::shared_ptr<Animator>;
	using Model3D = std::shared_ptr<Model>;
}