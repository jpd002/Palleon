#include <assert.h>
#include <float.h>
#include "PalleonEngine.h"
#include "BspFaceMesh.h"
#include "BspUtils.h"

static const unsigned int g_tessellationLevel = 5;
static const unsigned int g_patchVertexCount = (g_tessellationLevel + 1) * (g_tessellationLevel + 1);
static const unsigned int g_patchIndexCount = (g_tessellationLevel * g_tessellationLevel) * 6;

CBspFaceMesh::CBspFaceMesh(const CBspFile& bspFile, unsigned int faceIndex)
: m_vertices(NULL)
, m_indices(NULL)
, m_vertexCount(0)
, m_indexCount(0)
{
	const Bsp::FACE& face = bspFile.GetFaces()[faceIndex];
	if(face.type == Bsp::FACE_TYPE_MESH || face.type == Bsp::FACE_TYPE_POLYGON)
	{
		BuildMeshOrPolygon(bspFile, faceIndex);
	}
	else if(face.type == Bsp::FACE_TYPE_PATCH)
	{
		BuildPatch(bspFile, faceIndex);
	}
	else
	{
		//TODO: Implement billboards
		//assert(0);
	}
}

CBspFaceMesh::~CBspFaceMesh()
{
	delete [] m_vertices;
	delete [] m_indices;
}

uint8* CBspFaceMesh::GetVertices() const
{
	return m_vertices;
}

uint16* CBspFaceMesh::GetIndices() const
{
	return m_indices;
}

uint32 CBspFaceMesh::GetVertexCount() const
{
	return m_vertexCount;
}

uint32 CBspFaceMesh::GetIndexCount() const
{
	return m_indexCount;
}

void CBspFaceMesh::BuildMeshOrPolygon(const CBspFile& bspFile, unsigned int faceIndex)
{
	const Bsp::FACE& face = bspFile.GetFaces()[faceIndex];

	bool isLightMapped = (face.lightMapIndex != -1);

	m_vertexCount = face.vertexCount;
	m_indexCount = face.meshVertCount;

	Palleon::VERTEX_BUFFER_DESCRIPTOR bufferDesc = Palleon::GenerateVertexBufferDescriptor(0, 0, 
		Palleon::VERTEX_BUFFER_HAS_POS | Palleon::VERTEX_BUFFER_HAS_UV0 | Palleon::VERTEX_BUFFER_HAS_UV1 | Palleon::VERTEX_BUFFER_HAS_COLOR);
	const auto& posVertexItem = bufferDesc.GetVertexItem(Palleon::VERTEX_ITEM_ID_POSITION);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(Palleon::VERTEX_ITEM_ID_UV0);
	const auto& uv1VertexItem = bufferDesc.GetVertexItem(Palleon::VERTEX_ITEM_ID_UV1);
	const auto& colorVertexItem = bufferDesc.GetVertexItem(Palleon::VERTEX_ITEM_ID_COLOR);

	m_vertices = new uint8[bufferDesc.GetVertexSize() * m_vertexCount];
	m_indices = new uint16[m_indexCount];

	{
		uint8* vertexPtr = m_vertices;
		for(unsigned int i = 0; i < face.vertexCount; i++)
		{
			const Bsp::VERTEX& vertex = bspFile.GetVertices()[face.vertexIndex + i];
			CVector3 position = Bsp::GetVertexPosition(vertex);
			CVector2 texCoord0(vertex.texCoord0[0], vertex.texCoord0[1]);
			CVector2 texCoord1(vertex.texCoord1[0], vertex.texCoord1[1]);
			CColor color;
			if(!isLightMapped)
			{
				color = CColor(
					static_cast<float>(vertex.color[0]) / 255.f, 
					static_cast<float>(vertex.color[1]) / 255.f,
					static_cast<float>(vertex.color[2]) / 255.f,
					static_cast<float>(vertex.color[3]) / 255.f);
				color = color.MultiplyClamp(2);
			}
			else
			{
				color = CColor(1.0f, 1.0f, 1.0f, 1.0f);
			}
			*reinterpret_cast<CVector3*>(vertexPtr + posVertexItem->offset) = position;
			*reinterpret_cast<CVector2*>(vertexPtr + uv0VertexItem->offset) = texCoord0;
			*reinterpret_cast<CVector2*>(vertexPtr + uv1VertexItem->offset) = texCoord1;
			*reinterpret_cast<uint32*>(vertexPtr + colorVertexItem->offset) = Palleon::CGraphicDevice::ConvertColorToUInt32(color);
			vertexPtr += bufferDesc.GetVertexSize();
		}
	}

	{
		for(unsigned int i = 0; i < face.meshVertCount; i++)
		{
			uint32 index = bspFile.GetMeshVertices()[face.meshVertIndex + i];
			assert(index < 0x10000);
			m_indices[i] = static_cast<uint16>(index);
		}
	}
/*
	//Compute bounding box
	CVector3 maxPos(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	CVector3 minPos( FLT_MAX,  FLT_MAX,  FLT_MAX);
	for(unsigned int i = 0; i < face.vertexCount; i++)
	{
		const Bsp::VERTEX& vertex = file.GetVertices()[face.vertexIndex + i];
		CVector3 position = Bsp::GetVertexPosition(vertex);
		maxPos = maxPos.Max(position);
		minPos = minPos.Min(position);
	}
	CVector3 center = (maxPos + minPos) / 2;
*/
}

void CBspFaceMesh::BuildPatch(const CBspFile& bspFile, unsigned int faceIndex)
{
	const Bsp::FACE& face = bspFile.GetFaces()[faceIndex];
	assert(face.type == Bsp::FACE_TYPE_PATCH);

	unsigned int patchCountX = (face.patchSize[0] - 1) / 2;
	unsigned int patchCountY = (face.patchSize[1] - 1) / 2;
	unsigned int patchCount = patchCountX * patchCountY;

	assert(face.vertexCount == (face.patchSize[0] * face.patchSize[1]));

	unsigned int vertexCount = g_patchVertexCount * patchCount;
	unsigned int indexCount = g_patchIndexCount * patchCount;

	uint16 vertexIndex = 0;
	unsigned int indexIndex = 0;

	VertexArray patchVertices;
	IndexArray patchIndices;

	patchVertices.resize(vertexCount);
	patchIndices.resize(indexCount);

	for(unsigned int patchY = 0; patchY < patchCountY; patchY++)
	{
		for(unsigned int patchX = 0; patchX < patchCountX; patchX++)
		{
			unsigned int controlPtOriginX = 2 * patchX;
			unsigned int controlPtOriginY = 2 * patchY;

			unsigned int controlPt[9];
			for(unsigned int y = 0; y < 3; y++)
			{
				for(unsigned int x = 0; x < 3; x++)
				{
					unsigned int posX = controlPtOriginX + x;
					unsigned int posY = controlPtOriginY + y;
					unsigned int controlPtIndex = posX + (posY * face.patchSize[0]);
					assert(controlPtIndex < face.vertexCount);
					controlPt[x + (y * 3)] = face.vertexIndex + controlPtIndex;
				}
			}

			Tessellate(bspFile, controlPt, &patchVertices[vertexIndex], vertexIndex, &patchIndices[indexIndex]);
			vertexIndex += g_patchVertexCount;
			indexIndex += g_patchIndexCount;
		}
	}

	bool isLightMapped = (face.lightMapIndex != -1);

	m_vertexCount = vertexCount;
	m_indexCount = indexCount;

	Palleon::VERTEX_BUFFER_DESCRIPTOR bufferDesc = Palleon::GenerateVertexBufferDescriptor(vertexCount, indexCount, 
		Palleon::VERTEX_BUFFER_HAS_POS | Palleon::VERTEX_BUFFER_HAS_UV0 | Palleon::VERTEX_BUFFER_HAS_UV1 | Palleon::VERTEX_BUFFER_HAS_COLOR);
	const auto& posVertexItem = bufferDesc.GetVertexItem(Palleon::VERTEX_ITEM_ID_POSITION);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(Palleon::VERTEX_ITEM_ID_UV0);
	const auto& uv1VertexItem = bufferDesc.GetVertexItem(Palleon::VERTEX_ITEM_ID_UV1);
	const auto& colorVertexItem = bufferDesc.GetVertexItem(Palleon::VERTEX_ITEM_ID_COLOR);

	m_vertices = new uint8[bufferDesc.GetVertexSize() * m_vertexCount];
	m_indices = new uint16[m_indexCount];

	{
		uint8* vertexPtr = m_vertices;
		for(unsigned int i = 0; i < vertexCount; i++)
		{
			const Bsp::VERTEX& vertex = patchVertices[i];
			CVector3 position = Bsp::GetVertexPosition(vertex);
			CVector2 texCoord0(vertex.texCoord0[0], vertex.texCoord0[1]);
			CVector2 texCoord1(vertex.texCoord1[0], vertex.texCoord1[1]);
			CColor color;
			if(!isLightMapped)
			{
				color = CColor(vertex.color[0], vertex.color[1], vertex.color[2], vertex.color[3]);
			}
			else
			{
				color = CColor(1.0f, 1.0f, 1.0f, 1.0f);
			}
			*reinterpret_cast<CVector3*>(vertexPtr + posVertexItem->offset) = position;
			*reinterpret_cast<CVector2*>(vertexPtr + uv0VertexItem->offset) = texCoord0;
			*reinterpret_cast<CVector2*>(vertexPtr + uv1VertexItem->offset) = texCoord1;
			*reinterpret_cast<uint32*>(vertexPtr + colorVertexItem->offset) = Palleon::CGraphicDevice::ConvertColorToUInt32(color);
			vertexPtr += bufferDesc.GetVertexSize();
		}
	}

	{
		for(unsigned int i = 0; i < indexCount; i++)
		{
			m_indices[i] = patchIndices[i];
		}
	}
}

void CBspFaceMesh::Tessellate(const CBspFile& bspFile, unsigned int* controlPtIndices, Bsp::VERTEX* vertexOutput, uint16 indexBase, uint16* indexOutput)
{
	const Bsp::VertexArray& controlPt(bspFile.GetVertices());

	const unsigned int level = g_tessellationLevel;
	const unsigned int level1 = level + 1;

	for(unsigned int i = 0; i <= level; i++)
	{
		float a = static_cast<float>(i) / static_cast<float>(level);
		float b = (1 - a);

		vertexOutput[i] = 
			controlPt[controlPtIndices[0]] * (b * b) +
			controlPt[controlPtIndices[3]] * (2 * b * a) +
			controlPt[controlPtIndices[6]] * (a * a);
	}

	for(unsigned int i = 1; i <= level; i++)
	{
		float a = static_cast<float>(i) / static_cast<float>(level);
		float b = (1 - a);

		Bsp::VERTEX temp[3];

		for(unsigned int j = 0; j < 3; j++)
		{
			unsigned int k = 3 * j;
			temp[j] =
				controlPt[controlPtIndices[k + 0]] * (b * b) + 
				controlPt[controlPtIndices[k + 1]] * (2 * b * a) +
				controlPt[controlPtIndices[k + 2]] * (a * a);
		}

		for(unsigned int j = 0; j <= level; ++j) 
		{
			float a = static_cast<float>(j) / static_cast<float>(level);
			float b = (1 - a);

			vertexOutput[i * level1 + j] =
				temp[0] * (b * b) + 
				temp[1] * (2 * b * a) +
				temp[2] * (a * a);
		}
	}

	for(unsigned int row = 0; row < level; row++)
	{
		for(unsigned int col = 0; col < level; col++)
		{
			unsigned int corner00 = ((row + 0) * level1) + (col + 0);
			unsigned int corner01 = ((row + 0) * level1) + (col + 1);
			unsigned int corner10 = ((row + 1) * level1) + (col + 0);
			unsigned int corner11 = ((row + 1) * level1) + (col + 1);

			(*indexOutput++) = indexBase + corner10;
			(*indexOutput++) = indexBase + corner00;
			(*indexOutput++) = indexBase + corner01;

			(*indexOutput++) = indexBase + corner10;
			(*indexOutput++) = indexBase + corner01;
			(*indexOutput++) = indexBase + corner11;
		}
	}
}
