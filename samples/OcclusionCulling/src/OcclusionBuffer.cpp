#include "OcclusionBuffer.h"
#include <algorithm>
#include <cassert>

static const float s_cubePositions[24 * 3] =
{
	//Front
	-1,  1, -1,
	-1, -1, -1,
	 1,  1, -1,
	 1, -1, -1,
	 //Back
	 1, -1,  1,
	-1, -1,  1,
	 1,  1,  1,
	-1,  1,  1,
	 //Left
	-1, -1,  1,
	-1, -1, -1,
	-1,  1,  1,
	-1,  1, -1,
	 //Right
	 1,  1, -1,
	 1, -1, -1,
	 1,  1,  1,
	 1, -1,  1,
	//Bottom
	 1, -1, -1,
	-1, -1, -1,
	 1, -1,  1,
	-1, -1,  1,
	//Top
	-1,  1,  1,
	-1,  1, -1,
	 1,  1,  1,
	 1,  1, -1,
};

#define GENERATE_FACE_INDICES(i) (i * 4) + 1, (i * 4) + 0, (i * 4) + 2, (i * 4) + 1, (i * 4) + 2, (i * 4) + 3

static const uint16 s_cubeIndices[36] =
{
	GENERATE_FACE_INDICES(0),
	GENERATE_FACE_INDICES(1),
	GENERATE_FACE_INDICES(2),
	GENERATE_FACE_INDICES(3),
	GENERATE_FACE_INDICES(4),
	GENERATE_FACE_INDICES(5)
};

static const uint32 s_cubePrimitiveCount = 12;

COcclusionBuffer::COcclusionBuffer()
{
	m_buffer.resize(BUFFER_WIDTH * BUFFER_HEIGHT);
}

COcclusionBuffer::~COcclusionBuffer()
{

}

Framework::CBitmap COcclusionBuffer::MakeBufferBitmap() const
{
	Framework::CBitmap bitmap(BUFFER_WIDTH, BUFFER_HEIGHT, 32);
	for(unsigned int y = 0; y < BUFFER_HEIGHT; y++)
	{
		for(unsigned int x = 0; x < BUFFER_WIDTH; x++)
		{
			float bufferValue = m_buffer[x + (y * BUFFER_WIDTH)];
			bufferValue = std::max<float>(bufferValue, 0);
			bufferValue = std::min<float>(bufferValue, 1);
			uint8 colorValue = static_cast<uint8>(bufferValue * 255.f);
			bitmap.SetPixel(x, y, Framework::CColor(colorValue, colorValue, colorValue, 255));
		}
	}
	return bitmap;
}

void COcclusionBuffer::SetCamera(const Athena::CameraPtr& camera)
{
	m_camera = camera;
}

void COcclusionBuffer::Clear(float value)
{
	std::fill(std::begin(m_buffer), std::end(m_buffer), value);
}

void COcclusionBuffer::DrawMesh(const Athena::MeshPtr& mesh)
{
	auto worldViewProjMatrix = mesh->GetWorldTransformation() * m_camera->GetViewMatrix() * m_camera->GetProjectionMatrix();
	assert(mesh->GetPrimitiveType() == Athena::PRIMITIVE_TRIANGLE_LIST);
	assert(mesh->GetMaterial()->GetCullingMode() == Athena::CULLING_CCW);
	auto vertexBuffer = reinterpret_cast<const uint8*>(mesh->GetVertexBuffer()->GetShadowVertexBuffer());
	auto indexBuffer = mesh->GetVertexBuffer()->GetShadowIndexBuffer();
	const auto& vertexBufferDescriptor = mesh->GetVertexBuffer()->GetDescriptor();
	size_t vertexSize = vertexBufferDescriptor.GetVertexSize();
	const auto& posVertexItem = vertexBufferDescriptor.GetVertexItem(Athena::VERTEX_ITEM_ID_POSITION);
	for(unsigned int i = 0; i < mesh->GetPrimitiveCount(); i++)
	{
		auto vertexPtr1 = vertexBuffer + (indexBuffer[0] * vertexSize);
		auto vertexPtr2 = vertexBuffer + (indexBuffer[1] * vertexSize);
		auto vertexPtr3 = vertexBuffer + (indexBuffer[2] * vertexSize);
		indexBuffer += 3;
		TransformAndDrawTriangle(
			*reinterpret_cast<const CVector3*>(vertexPtr1 + posVertexItem->offset),
			*reinterpret_cast<const CVector3*>(vertexPtr2 + posVertexItem->offset),
			*reinterpret_cast<const CVector3*>(vertexPtr3 + posVertexItem->offset),
			worldViewProjMatrix, DRAW_MODE_WRITE);
	}
}

bool COcclusionBuffer::TestMesh(const Athena::MeshPtr& mesh)
{
	auto worldViewProjMatrix = mesh->GetWorldTransformation() * m_camera->GetViewMatrix() * m_camera->GetProjectionMatrix();
	auto boundingSphere = mesh->GetBoundingSphere();
	//If bounding sphere has no radius, we assume that it can't be tested and that we should render it anyways.
	if(boundingSphere.radius == 0) return true;
	auto indexBuffer = s_cubeIndices;
	auto vertexBuffer = s_cubePositions;
	uint32 vertexSize = 3;
	for(unsigned int i = 0; i < s_cubePrimitiveCount; i++)
	{
		auto vertexPtr1 = vertexBuffer + (indexBuffer[0] * vertexSize);
		auto vertexPtr2 = vertexBuffer + (indexBuffer[1] * vertexSize);
		auto vertexPtr3 = vertexBuffer + (indexBuffer[2] * vertexSize);
		indexBuffer += 3;
		bool testPassed = TransformAndDrawTriangle(
			*reinterpret_cast<const CVector3*>(vertexPtr1) * boundingSphere.radius,
			*reinterpret_cast<const CVector3*>(vertexPtr2) * boundingSphere.radius,
			*reinterpret_cast<const CVector3*>(vertexPtr3) * boundingSphere.radius,
			worldViewProjMatrix, DRAW_MODE_TEST);
		if(testPassed) return true;
	}
	return false;
}

bool COcclusionBuffer::TransformAndDrawTriangle(const CVector3& v1, const CVector3& v2, const CVector3& v3, const CMatrix4& worldViewProjMatrix, DRAW_MODE drawMode)
{
	auto pos1 = CVector4(v1, 1);
	auto pos2 = CVector4(v2, 1);
	auto pos3 = CVector4(v3, 1);
	pos1 = worldViewProjMatrix * pos1;
	pos2 = worldViewProjMatrix * pos2;
	pos3 = worldViewProjMatrix * pos3;
	auto pos21 = pos2.xyz() - pos1.xyz();
	auto pos31 = pos3.xyz() - pos1.xyz();
	auto normal = pos21.Cross(pos31);
	//Backface cull
	if(normal.z > 0)
	{
		return false;
	}
	auto pos1Screen = pos1.xyz() / pos1.w;
	auto pos2Screen = pos2.xyz() / pos2.w;
	auto pos3Screen = pos3.xyz() / pos3.w;
	pos1Screen.y *= -1;
	pos2Screen.y *= -1;
	pos3Screen.y *= -1;
	pos1Screen.x = ((pos1Screen.x + 1) / 2) * BUFFER_WIDTH; pos1Screen.y = ((pos1Screen.y + 1) / 2) * BUFFER_HEIGHT;
	pos2Screen.x = ((pos2Screen.x + 1) / 2) * BUFFER_WIDTH; pos2Screen.y = ((pos2Screen.y + 1) / 2) * BUFFER_HEIGHT;
	pos3Screen.x = ((pos3Screen.x + 1) / 2) * BUFFER_WIDTH; pos3Screen.y = ((pos3Screen.y + 1) / 2) * BUFFER_HEIGHT;
	bool testPassed = DrawTriangle(pos1Screen, pos2Screen, pos3Screen, drawMode);
	return testPassed;
}

bool COcclusionBuffer::DrawTriangle(const CVector3& v1, const CVector3& v2, const CVector3& v3, DRAW_MODE drawMode)
{
	EDGE edges[3] =
	{
		EDGE(v1, v2),
		EDGE(v2, v3),
		EDGE(v3, v1)
	};

	auto longEdgeIterator = std::max_element(std::begin(edges), std::end(edges), 
		[](const EDGE& e1, const EDGE& e2)
		{
			return e1.GetDeltaY() < e2.GetDeltaY();
		}
	);

	for(auto edgeIterator = std::begin(edges); edgeIterator != std::end(edges); edgeIterator++)
	{
		if(edgeIterator == longEdgeIterator) continue;
		bool testPassed = DrawSpansBetweenEdges(*longEdgeIterator, *edgeIterator, drawMode);
		if(drawMode == DRAW_MODE_TEST && testPassed)
		{
			return true;
		}
	}
	
	return false;
}

bool COcclusionBuffer::DrawSpansBetweenEdges(const EDGE& e1, const EDGE& e2, DRAW_MODE drawMode)
{
	float e1Dy = e1.GetDeltaY();
	if(e1Dy == 0) return false;

	float e2Dy = e2.GetDeltaY();
	if(e2Dy == 0) return false;

	float e1Dx = e1.GetDeltaX(); float e1Dz = e1.GetDeltaZ();
	float e2Dx = e2.GetDeltaX(); float e2Dz = e2.GetDeltaZ();
	
	float factor1 = (e2.v1.y - e1.v1.y) / e1Dy;
	float factorStep1 = 1.0f / e1Dy;
	float factor2 = 0;
	float factorStep2 = 1.0f / e2Dy;

	int intY1 = static_cast<int>(e2.v1.y);
	int intY2 = static_cast<int>(e2.v2.y);
	assert(intY1 <= intY2);
	for(int y = intY1; y < intY2; y++)
	{
		if(y >= 0 && y < BUFFER_HEIGHT)
		{
			SPAN span(
				e1.v1.x + e1Dx * factor1, e1.v1.z + e1Dz * factor1,
				e2.v1.x + e2Dx * factor2, e2.v1.z + e2Dz * factor2);

			bool testPassed = DrawSpan(y, span, drawMode);
			if(drawMode == DRAW_MODE_TEST && testPassed)
			{
				return true;
			}
		}

		factor1 += factorStep1;
		factor2 += factorStep2;
	}
	
	return false;
}

bool COcclusionBuffer::DrawSpan(int y, const SPAN& span, DRAW_MODE drawMode)
{
	float dx = span.GetDeltaX();
	if(dx == 0) return false;

	float dz = span.GetDeltaZ();
	float factor = 0;
	float factorStep = 1.f / dx;

	int intX1 = static_cast<int>(span.x1);
	int intX2 = static_cast<int>(span.x2);
	assert(intX1 <= intX2);
	for(int x = intX1; x < intX2; x++)
	{
		if(x >= 0 && x < BUFFER_WIDTH)
		{
			float z = span.z1 + (dz * factor);
			float& bufZ = m_buffer[(y * BUFFER_WIDTH) + x];
			//Depth Test (LESS)
			if(z < bufZ)
			{
				if(drawMode == DRAW_MODE_WRITE)
				{
//					bufZ = z;
					bufZ = 0;
				}
				else
				{
					return true;
				}
			}
		}
		factor += factorStep;
	}

	return false;
}

COcclusionBuffer::EDGE::EDGE(const CVector3& inputV1, const CVector3& inputV2)
: v1(inputV1), v2(inputV2)
{
	if(v1.y > v2.y)
	{
		std::swap(v1, v2);
	}
}

float COcclusionBuffer::EDGE::GetDeltaX() const
{
	return v2.x - v1.x;
}

float COcclusionBuffer::EDGE::GetDeltaY() const
{
	return v2.y - v1.y;
}

float COcclusionBuffer::EDGE::GetDeltaZ() const
{
	return v2.z - v1.z;
}

COcclusionBuffer::SPAN::SPAN(float inputX1, float inputZ1, float inputX2, float inputZ2)
: x1(inputX1), z1(inputZ1), x2(inputX2), z2(inputZ2)
{
	if(x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(z1, z2);
	}
}

float COcclusionBuffer::SPAN::GetDeltaX() const
{
	return x2 - x1;
}

float COcclusionBuffer::SPAN::GetDeltaZ() const
{
	return z2 - z1;
}
