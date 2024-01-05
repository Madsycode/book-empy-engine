#pragma once
#include "Vertex.h"

namespace Empy
{
	template <typename Vertex> struct Mesh
	{
		EMPY_INLINE Mesh(const MeshData<Vertex>& data) 
		{
			// check vertices
			if(data.Vertices.empty())
            {
                EMPY_ERROR("empty mesh data!");
                return;
            }

			// number of vertices and indices
			m_NbrVertex = data.Vertices.size();
			m_NbrIndex = data.Indices.size();

			// generate vrtex buffer array
			glGenVertexArrays(1, &m_BufferID);

			// activate/bind vertex array
			glBindVertexArray(m_BufferID);

			// create vertex buffer
			uint32_t VBO = 0u;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, m_NbrVertex * 
			sizeof(Vertex), data.Vertices.data(), GL_STATIC_DRAW);

			// create index buffer 
			if(m_NbrIndex != 0u) 
			{
				uint32_t EBO = 0u;
				glGenBuffers(1, &EBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_NbrIndex * 
				sizeof(uint32_t), data.Indices.data(), GL_STATIC_DRAW);
			}

			// handle vertex types
			if (TypeID<Vertex>() == TypeID<ShadedVertex>()) 
			{
				SetAttribute(0, 3, (void*)offsetof(ShadedVertex, Position));
				SetAttribute(1, 3, (void*)offsetof(ShadedVertex, Normal));
				SetAttribute(2, 2, (void*)offsetof(ShadedVertex, UVs));
				SetAttribute(3, 3, (void*)offsetof(ShadedVertex, Tangent));
				SetAttribute(4, 3, (void*)offsetof(ShadedVertex, Bitangent));
			}
			else if (TypeID<Vertex>() == TypeID<FlatVertex>()) 
			{
				SetAttribute(0, 3, (void*)offsetof(FlatVertex, Position));
				SetAttribute(1, 4, (void*)offsetof(FlatVertex, Color));
			}
			else if (TypeID<Vertex>() == TypeID<QuadVertex>()) 
			{
				SetAttribute(0, 4, (void*)offsetof(QuadVertex, Data));
			}	
			else 
			{
				EMPY_ERROR(false && "invalid vertex type!");
			}

			// unbind vertext array
			glBindVertexArray(0);
		}
       	
		EMPY_INLINE void Draw(uint32_t mode) 
		{
			glBindVertexArray(m_BufferID);
			if(m_NbrIndex != 0u) 
			{
				glDrawElements(mode, m_NbrIndex, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				return;
			}
			glDrawArrays(mode, 0, m_NbrVertex);
			glBindVertexArray(0);
		}

        EMPY_INLINE ~Mesh() 
		{ 
			glDeleteVertexArrays(1, &m_BufferID); 
		}	

    private:
        EMPY_INLINE void SetAttribute(uint32_t index, int32_t size, const void* value) 
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, sizeof(Vertex), value);
		}
			
	private:
		uint32_t m_NbrVertex = 0u;
		uint32_t m_NbrIndex = 0u;
		uint32_t m_BufferID = 0u;
	};

	// 3d mesh
    using ShadedMesh = Mesh<ShadedVertex>;
	using Mesh3D = std::shared_ptr<ShadedMesh>;
}