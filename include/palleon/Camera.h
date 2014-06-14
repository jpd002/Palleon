#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <memory>
#include "Matrix4.h"
#include "Vector3.h"
#include "Frustum.h"

namespace Palleon
{
	class CCamera;
	typedef std::shared_ptr<CCamera> CameraPtr;

	enum HANDEDNESS
	{
		HANDEDNESS_LEFTHANDED,
		HANDEDNESS_RIGHTHANDED,
	};

	class CCamera
	{
	public:
							CCamera();
		virtual				~CCamera();

		static CameraPtr	Create();

		void				SetupOrthoCamera(float, float);

		void				SetOrthoProjection(float width, float height, float near, float far);
		void				SetPerspectiveProjection(float fovY, float aspectRatio, float near, float far, HANDEDNESS = HANDEDNESS_LEFTHANDED);

		void				LookAt(const CVector3& eye, const CVector3& target, const CVector3& up, HANDEDNESS = HANDEDNESS_LEFTHANDED);
		void				SetViewMatrix(const CMatrix4&);

		const CMatrix4&		GetProjectionMatrix() const;
		const CMatrix4&		GetViewMatrix() const;

		CFrustum			GetFrustum() const;

	protected:
		CMatrix4			m_viewMatrix;
		CMatrix4			m_projMatrix;
	};
}

#endif
