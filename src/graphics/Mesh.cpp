#include "palleon/graphics/Mesh.h"
#include "palleon/graphics/GraphicDevice.h"
#include "palleon/MathOps.h"

using namespace Palleon;

CMesh::CMesh()
: m_primitiveType(PRIMITIVE_INVALID)
, m_primitiveCount(0)
, m_material(CMaterial::Create())
, m_effectProvider(CGraphicDevice::GetInstance().GetDefaultEffectProvider())
, m_isPeggedToOrigin(false)
{

}

CMesh::~CMesh()
{

}

VertexBufferPtr CMesh::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

MaterialPtr CMesh::GetMaterial() const
{
	return m_material;
}

void CMesh::SetMaterial(const MaterialPtr& material)
{
	m_material = material;
}

EffectProviderPtr CMesh::GetEffectProvider() const
{
	return m_effectProvider;
}

void CMesh::SetEffectProvider(const EffectProviderPtr& effectProvider)
{
	m_effectProvider = effectProvider;
}

PRIMITIVE_TYPE CMesh::GetPrimitiveType() const
{
	return m_primitiveType;
}

uint32 CMesh::GetPrimitiveCount() const
{
	return m_primitiveCount;
}

CSphere CMesh::GetBoundingSphere() const
{
	return m_boundingSphere;
}

void CMesh::SetBoundingSphere(const CSphere& boundingSphere)
{
	m_boundingSphere = boundingSphere;
}

CSphere CMesh::GetWorldBoundingSphere() const
{
	return Transform(m_boundingSphere, GetWorldTransformation());
}

bool CMesh::GetIsPeggedToOrigin() const
{
	return m_isPeggedToOrigin;
}

void CMesh::SetIsPeggedToOrigin(bool isPeggedToOrigin)
{
	m_isPeggedToOrigin = isPeggedToOrigin;
}
