#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <memory>
#include "Matrix4.h"
#include "Vector3.h"

namespace Athena
{
	class CCamera;
	typedef std::tr1::shared_ptr<CCamera> CameraPtr;

	class CCamera
	{
	public:
		virtual				~CCamera();

		static CameraPtr	Create();

		void				SetupOrthoCamera(float, float);

		void				SetOrthoProjection(float width, float height, float near, float far);
		void				SetPerspectiveProjection(float fovY, float aspectRatio, float near, float far);

		void				LookAt(const CVector3& eye, const CVector3& target, const CVector3& up);
		void				SetViewMatrix(const CMatrix4&);

		const CMatrix4&		GetProjectionMatrix() const;
		const CMatrix4&		GetViewMatrix() const;

	protected:
							CCamera();

		CMatrix4			m_viewMatrix;
		CMatrix4			m_projMatrix;
	};
}

#endif
