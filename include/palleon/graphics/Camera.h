#pragma once

#include <memory>
#include "math/Matrix4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Ray.h"
#include "palleon/Frustum.h"

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
		void				SetProjectionMatrix(const CMatrix4&);

		void				LookAt(const CVector3& eye, const CVector3& target, const CVector3& up, HANDEDNESS = HANDEDNESS_LEFTHANDED);
		void				SetViewMatrix(const CMatrix4&);

		const CMatrix4&		GetProjectionMatrix() const;
		const CMatrix4&		GetViewMatrix() const;

		CFrustum			GetFrustum() const;

		CRay				Unproject(const CVector2&) const;

	protected:
		CMatrix4			m_viewMatrix;
		CMatrix4			m_projMatrix;
	};
}
