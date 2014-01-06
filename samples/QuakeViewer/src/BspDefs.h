#ifndef _BSPDEFS_H_
#define _BSPDEFS_H_

#include "Types.h"
#include <vector>

namespace Bsp
{
	struct TEXTURE
	{
		char		name[64];
		uint32		flags;
		uint32		contents;
	};

	struct PLANE
	{
		float		normal[3];
		float		distance;
	};

	struct NODE
	{
		uint32		plane;
		int32		children[2];
		int32		mins[3];
		int32		maxs[3];
	};

	struct LEAF
	{
		uint32		cluster;
		uint32		area;
		int32		mins[3];
		int32		maxs[3];
		uint32		leafFaceIndex;
		uint32		leafFaceCount;
		uint32		leafBrushIndex;
		uint32		leafBrushCount;
	};

	struct VERTEX
	{
		float		position[3];
		float		texCoord0[2];
		float		texCoord1[2];
		float		normal[3];
		uint8		color[4];

		VERTEX operator *(float value) const
		{
			VERTEX result(*this);
			
			result.position[0] *= value;
			result.position[1] *= value;
			result.position[2] *= value;

			result.texCoord0[0] *= value;
			result.texCoord0[1] *= value;

			result.texCoord1[0] *= value;
			result.texCoord1[1] *= value;
		
			result.normal[0] *= value;
			result.normal[1] *= value;
			result.normal[2] *= value;

			result.color[0] = static_cast<uint8>(static_cast<float>(result.color[0]) * value);
			result.color[1] = static_cast<uint8>(static_cast<float>(result.color[1]) * value);
			result.color[2] = static_cast<uint8>(static_cast<float>(result.color[2]) * value);
			result.color[3] = static_cast<uint8>(static_cast<float>(result.color[3]) * value);

			return result;
		}

		VERTEX operator +(const VERTEX& rhs) const
		{
			VERTEX result;

			result.position[0] = position[0] + rhs.position[0];
			result.position[1] = position[1] + rhs.position[1];
			result.position[2] = position[2] + rhs.position[2];

			result.texCoord0[0] = texCoord0[0] + rhs.texCoord0[0];
			result.texCoord0[1] = texCoord0[1] + rhs.texCoord0[1];

			result.texCoord1[0] = texCoord1[0] + rhs.texCoord1[0];
			result.texCoord1[1] = texCoord1[1] + rhs.texCoord1[1];

			result.normal[0] = normal[0] + rhs.normal[0];
			result.normal[1] = normal[1] + rhs.normal[1];
			result.normal[2] = normal[2] + rhs.normal[2];

			for(unsigned int i = 0; i < 4; i++)
			{
				uint32 color0 = color[i];
				uint32 color1 = rhs.color[i];
				result.color[i] = static_cast<uint8>(color0 + color1);
			}

			return result;
		}
	};

	struct EFFECT
	{
		char		name[64];
		uint32		brush;
		uint32		unknown;
	};

	struct FACE
	{
		uint32		textureIndex;
		uint32		effectIndex;
		uint32		type;
		uint32		vertexIndex;
		uint32		vertexCount;
		uint32		meshVertIndex;
		uint32		meshVertCount;
		uint32		lightMapIndex;
		uint32		lightMapStart[2];
		uint32		lightMapSize[2];
		float		lightMapOrigin[3];
		float		lightMapVectors[2][3];
		float		normal[3];
		uint32		patchSize[2];
	};

	struct LIGHTMAP
	{
		uint8		colors[128][128][3];
	};

	struct VISDATA
	{
		uint32				vectorCount;
		uint32				vectorSize;
		std::vector<uint8>	vectors;
	};

	enum FACE_TYPES
	{
		FACE_TYPE_POLYGON = 1,
		FACE_TYPE_PATCH = 2,
		FACE_TYPE_MESH = 3,
		FACE_TYPE_BILLBOARD = 4,
	};

	typedef std::vector<char> EntityArray;
	typedef std::vector<TEXTURE> TextureArray;
	typedef std::vector<PLANE> PlaneArray;
	typedef std::vector<NODE> NodeArray;
	typedef std::vector<LEAF> LeafArray;
	typedef std::vector<uint32> LeafFaceArray;
	typedef std::vector<VERTEX> VertexArray;
	typedef std::vector<uint32> MeshVertexArray;
	typedef std::vector<EFFECT> EffectArray;
	typedef std::vector<FACE> FaceArray;
	typedef std::vector<LIGHTMAP> LightMapArray;
}

#endif
