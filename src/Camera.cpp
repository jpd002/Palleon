#include "athena/Camera.h"

using namespace Athena;

CCamera::CCamera()
{

}

CCamera::~CCamera()
{

}

CameraPtr CCamera::Create()
{
	return CameraPtr(new CCamera());
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
	proj(0, 0) =  2.0f / width;
	proj(1, 1) =  2.0f / height;
	proj(2, 2) =  1.0f / (far - near);
	proj(3, 3) = 1;

	proj(3, 2) = near / (near - far);

	m_projMatrix = proj;
}

void CCamera::SetPerspectiveProjection(float fovY, float aspectRatio, float near, float far)
{
	float yScale = 1 / tan(fovY / 2);
	float xScale = yScale / aspectRatio;

	CMatrix4 proj;
	proj(0, 0) = xScale;
	proj(1, 1) = yScale;
	proj(2, 2) = far / (far - near);
	proj(3, 3) = 0;

	proj(3, 2) = -near * far / (far - near);
	proj(2, 3) = 1;

	m_projMatrix = proj;
}

void CCamera::LookAt(const CVector3& eye, const CVector3& target, const CVector3& up)
{
	//zaxis = normal(At - Eye)
	//xaxis = normal(cross(Up, zaxis))
	//yaxis = cross(zaxis, xaxis)

	CMatrix4 view;

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
