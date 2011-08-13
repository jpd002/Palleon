#include "athena/Mesh.h"

using namespace Athena;

CMesh::CMesh()
: m_primitiveType(PRIMITIVE_INVALID)
, m_primitiveCount(0)
, m_color(1.0f, 1.0f, 1.0f, 1.0f)
, m_material(CMaterial::Create())
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

PRIMITIVE_TYPE CMesh::GetPrimitiveType() const
{
	return m_primitiveType;
}

uint32 CMesh::GetPrimitiveCount() const
{
	return m_primitiveCount;
}

CColor CMesh::GetColor() const
{
	return m_color;
}

void CMesh::SetColor(const CColor& color)
{
	m_color = color;
}
