#pragma once

#include "SceneNode.h"
#include "VertexBuffer.h"
#include "Material.h"
#include "EffectProvider.h"
#include "Sphere.h"

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
		virtual					~CMesh();

		VertexBufferPtr			GetVertexBuffer() const;

		MaterialPtr				GetMaterial() const;
		void					SetMaterial(const MaterialPtr&);

		EffectProviderPtr		GetEffectProvider() const;
		void					SetEffectProvider(const EffectProviderPtr&);

		PRIMITIVE_TYPE			GetPrimitiveType() const;
		uint32					GetPrimitiveCount() const;

		CSphere					GetBoundingSphere() const;
		void					SetBoundingSphere(const CSphere&);

		CSphere					GetWorldBoundingSphere() const;

		bool					GetIsPeggedToOrigin() const;
		void					SetIsPeggedToOrigin(bool);

	protected:
								CMesh();

		VertexBufferPtr			m_vertexBuffer;
		MaterialPtr				m_material;
		EffectProviderPtr		m_effectProvider;
		PRIMITIVE_TYPE			m_primitiveType;
		uint32					m_primitiveCount;
		CSphere					m_boundingSphere;
		bool					m_isPeggedToOrigin;
	};

	typedef std::shared_ptr<CMesh> MeshPtr;
}
