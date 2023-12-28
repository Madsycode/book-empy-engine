#pragma once
#include "../Buffers/Mesh.h"
#include <assimp/quaternion.h>
#include <assimp/matrix4x4.h>
#include <assimp/color4.h>

namespace Empy
{
	EMPY_INLINE static glm::vec3 AssimpToVec3(const aiVector3D& v) 
    {
		return glm::vec3(v.x, v.y, v.z);
	}

	EMPY_INLINE static glm::vec4 AssimpToVec4(const aiColor4D& c) 
    {
		return glm::vec4(c.r, c.g, c.b, c.a);
	}

	EMPY_INLINE static glm::quat AssimpToQuat(const aiQuaternion& q) 
    {
		return glm::quat(q.w, q.x, q.y, q.z);
	}

	EMPY_INLINE static glm::mat4 AssimpToMat4(const aiMatrix4x4& from) 
    {
		glm::mat4 to;
		//the a, b, c, d in assimp is the row; the 1, 2, 3, 4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}
}