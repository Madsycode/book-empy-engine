#pragma once
#include <assimp/postprocess.h>
#include <assimp/quaternion.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Helper.h"

namespace Empy
{
    struct Model
    {
        EMPY_INLINE Model() = default;

        EMPY_INLINE Model(const std::string& filename)
        {
            Load(filename);
        }

        EMPY_INLINE void Load(const std::string& filename)
        {
			uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure |
			aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals | 
			aiProcess_GenUVCoords | aiProcess_FlipUVs;

            Assimp::Importer importer;
			const aiScene* ai_scene = importer.ReadFile(filename, flags);

			if (!ai_scene || ai_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
				EMPY_ERROR("failed to load model-{}", importer.GetErrorString());
				return;
			}

            // parse all meshes
			ParseNode(ai_scene, ai_scene->mRootNode);
        }

        EMPY_INLINE void Draw(uint32_t mode)
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
        std::vector<Mesh3D> m_Meshes;
    };

	// 3d model typedef
	using Model3D = std::shared_ptr<Model>;
}