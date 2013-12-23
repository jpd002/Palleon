#include "athena/Mesh.h"
#include "athena/GraphicDevice.h"

using namespace Athena;

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

bool CMesh::GetIsPeggedToOrigin() const
{
	return m_isPeggedToOrigin;
}

void CMesh::SetIsPeggedToOrigin(bool isPeggedToOrigin)
{
	m_isPeggedToOrigin = isPeggedToOrigin;
}
