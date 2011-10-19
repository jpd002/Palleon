#ifndef _MESH_H_
#define _MESH_H_

#include "SceneNode.h"
#include "VertexBuffer.h"
#include "Material.h"

namespace Athena
{
	enum PRIMITIVE_TYPE
	{
		PRIMITIVE_INVALID = 0,
		PRIMITIVE_TRIANGLE_LIST,
		PRIMITIVE_TRIANGLE_STRIP
	};

	class CMesh : public CSceneNode
	{
	public:
		virtual				~CMesh();

		VertexBufferPtr		GetVertexBuffer() const;

		MaterialPtr			GetMaterial() const;

		PRIMITIVE_TYPE		GetPrimitiveType() const;
		uint32				GetPrimitiveCount() const;

	protected:
							CMesh();

		VertexBufferPtr		m_vertexBuffer;
		MaterialPtr			m_material;
		PRIMITIVE_TYPE		m_primitiveType;
		uint32				m_primitiveCount;
	};

	typedef std::tr1::shared_ptr<CMesh> MeshPtr;
}

#endif
