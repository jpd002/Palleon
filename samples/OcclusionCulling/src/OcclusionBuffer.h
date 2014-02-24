#pragma once

#include <vector>
#include "bitmap/Bitmap.h"
#include "AthenaEngine.h"

class COcclusionBuffer
{
public:
							COcclusionBuffer();
	virtual					~COcclusionBuffer();

	Framework::CBitmap		MakeBufferBitmap() const;

	void					SetCamera(const Athena::CameraPtr&);

	void					Clear(float);
	void					DrawMesh(const Athena::MeshPtr&);
	bool					TestMesh(const Athena::MeshPtr&);

private:
	enum
	{
		BUFFER_WIDTH = 320,
		BUFFER_HEIGHT = 256,
	};

	struct EDGE
	{
		EDGE(const CVector3&, const CVector3&);

		float GetDeltaX() const;
		float GetDeltaY() const;
		float GetDeltaZ() const;

		CVector3 v1;
		CVector3 v2;
	};

	struct SPAN
	{
		SPAN(float, float, float, float);

		float GetDeltaX() const;
		float GetDeltaZ() const;

		float x1, z1;
		float x2, z2;
	};

	enum DRAW_MODE
	{
		DRAW_MODE_WRITE,
		DRAW_MODE_TEST
	};

	bool					TransformAndDrawTriangle(const CVector3&, const CVector3&, const CVector3&, const CMatrix4&, DRAW_MODE);
	bool					DrawTriangle(const CVector3&, const CVector3&, const CVector3&, DRAW_MODE);
	bool					DrawSpansBetweenEdges(const EDGE&, const EDGE&, DRAW_MODE);
	bool					DrawSpan(int, const SPAN&, DRAW_MODE);

	std::vector<float>		m_buffer;
	Athena::CameraPtr		m_camera;
};
