#include "palleon/Camera.h"

using namespace Palleon;

CCamera::CCamera()
: m_viewMatrix(CMatrix4::MakeIdentity())
, m_projMatrix(CMatrix4::MakeIdentity())
{

}

CCamera::~CCamera()
{

}

CameraPtr CCamera::Create()
{
	return std::make_shared<CCamera>();
}

void CCamera::SetupOrthoCamera(float width, float height)
{
	SetOrthoProjection(width, height, 0, 1);
	LookAt(
		CVector3(width / 2, height / 2, 1),
		CVector3(width / 2, height / 2, 0),
		CVector3(0, -1, 0)
		);
}

void CCamera::SetOrthoProjection(float width, float height, float near, float far)
{
	CMatrix4 proj;
	proj.Clear();

	proj(0, 0) =  2.0f / width;
	proj(1, 1) =  2.0f / height;
	proj(2, 2) =  1.0f / (far - near);
	proj(3, 3) = 1;

	proj(3, 2) = near / (near - far);

	m_projMatrix = proj;
}

void CCamera::SetPerspectiveProjection(float fovY, float aspectRatio, float near, float far, HANDEDNESS handedness)
{
	float yScale = 1 / tan(fovY / 2);
	float xScale = yScale / aspectRatio;

	CMatrix4 proj;
	proj.Clear();

	if(handedness == HANDEDNESS_LEFTHANDED)
	{
		proj(0, 0) = xScale;
		proj(1, 1) = yScale;
		proj(2, 2) = far / (far - near);
		proj(3, 3) = 0;

		proj(3, 2) = -near * far / (far - near);
		proj(2, 3) = 1;
	}
	else
	{
		proj(0, 0) = xScale;
		proj(1, 1) = yScale;
		proj(2, 2) = far / (near - far);
		proj(3, 3) = 0;

		proj(3, 2) = near * far / (near - far);
		proj(2, 3) = -1;
	}

	m_projMatrix = proj;
}

void CCamera::SetProjectionMatrix(const CMatrix4& projMatrix)
{
	m_projMatrix = projMatrix;
}

void CCamera::LookAt(const CVector3& eye, const CVector3& target, const CVector3& up, HANDEDNESS handedness)
{
	CMatrix4 view;
	view.Clear();

	if(handedness == HANDEDNESS_LEFTHANDED)
	{
		//zaxis = normal(At - Eye)
		//xaxis = normal(cross(Up, zaxis))
		//yaxis = cross(zaxis, xaxis)

		CVector3 axisZ = (target - eye).Normalize();
		CVector3 axisX = (up.Cross(axisZ)).Normalize();
		CVector3 axisY = axisZ.Cross(axisX);

		view(0, 0) = axisX.x;
		view(1, 0) = axisX.y;
		view(2, 0) = axisX.z;
		view(3, 0) = -axisX.Dot(eye);

		view(0, 1) = axisY.x;
		view(1, 1) = axisY.y;
		view(2, 1) = axisY.z;
		view(3, 1) = -axisY.Dot(eye);

		view(0, 2) = axisZ.x;
		view(1, 2) = axisZ.y;
		view(2, 2) = axisZ.z;
		view(3, 2) = -axisZ.Dot(eye);

		view(3, 3) = 1;
	}
	else
	{
		//zaxis = normal(Eye - At)
		//xaxis = normal(cross(Up, zaxis))
		//yaxis = cross(zaxis, xaxis)

		CVector3 axisZ = (eye - target).Normalize();
		CVector3 axisX = (up.Cross(axisZ)).Normalize();
		CVector3 axisY = axisZ.Cross(axisX);

		view(0, 0) = axisX.x;
		view(1, 0) = axisX.y;
		view(2, 0) = axisX.z;
		view(3, 0) = -axisX.Dot(eye);

		view(0, 1) = axisY.x;
		view(1, 1) = axisY.y;
		view(2, 1) = axisY.z;
		view(3, 1) = -axisY.Dot(eye);

		view(0, 2) = axisZ.x;
		view(1, 2) = axisZ.y;
		view(2, 2) = axisZ.z;
		view(3, 2) = -axisZ.Dot(eye);

		view(3, 3) = 1;
	}

	m_viewMatrix = view;
}

void CCamera::SetViewMatrix(const CMatrix4& viewMatrix)
{
	m_viewMatrix = viewMatrix;
}

const CMatrix4& CCamera::GetProjectionMatrix() const
{
	return m_projMatrix;
}

const CMatrix4& CCamera::GetViewMatrix() const
{
	return m_viewMatrix;
}

CFrustum CCamera::GetFrustum() const
{
	CMatrix4 viewProjection = m_viewMatrix * m_projMatrix;
	CFrustum result;

	// Left plane
	result.planes[0].a = viewProjection(0, 3) + viewProjection(0, 0);
	result.planes[0].b = viewProjection(1, 3) + viewProjection(1, 0);
	result.planes[0].c = viewProjection(2, 3) + viewProjection(2, 0);
	result.planes[0].d = viewProjection(3, 3) + viewProjection(3, 0);

	// Right plane
	result.planes[1].a = viewProjection(0, 3) - viewProjection(0, 0);
	result.planes[1].b = viewProjection(1, 3) - viewProjection(1, 0);
	result.planes[1].c = viewProjection(2, 3) - viewProjection(2, 0);
	result.planes[1].d = viewProjection(3, 3) - viewProjection(3, 0);

	// Top plane
	result.planes[2].a = viewProjection(0, 3) - viewProjection(0, 1);
	result.planes[2].b = viewProjection(1, 3) - viewProjection(1, 1);
	result.planes[2].c = viewProjection(2, 3) - viewProjection(2, 1);
	result.planes[2].d = viewProjection(3, 3) - viewProjection(3, 1);

	// Bottom plane
	result.planes[3].a = viewProjection(0, 3) + viewProjection(0, 1);
	result.planes[3].b = viewProjection(1, 3) + viewProjection(1, 1);
	result.planes[3].c = viewProjection(2, 3) + viewProjection(2, 1);
	result.planes[3].d = viewProjection(3, 3) + viewProjection(3, 1);

	// Near plane
	result.planes[4].a = viewProjection(0, 2);
	result.planes[4].b = viewProjection(1, 2);
	result.planes[4].c = viewProjection(2, 2);
	result.planes[4].d = viewProjection(3, 2);

	// Far plane
	result.planes[5].a = viewProjection(0, 3) - viewProjection(0, 2);
	result.planes[5].b = viewProjection(1, 3) - viewProjection(1, 2);
	result.planes[5].c = viewProjection(2, 3) - viewProjection(2, 2);
	result.planes[5].d = viewProjection(3, 3) - viewProjection(3, 2);

	for(unsigned int i = 0; i < 6; i++)
	{
		result.planes[i] = result.planes[i].Normalize();
	}

	return result;
}
