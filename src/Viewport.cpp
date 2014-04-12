#include "palleon/Viewport.h"

using namespace Palleon;

CViewport::CViewport()
{
	m_sceneRoot = CSceneNode::Create();
}

CViewport::~CViewport()
{

}

ViewportPtr CViewport::Create()
{
	return ViewportPtr(new CViewport());
}

CameraPtr CViewport::GetCamera() const
{
	return m_camera;
}

void CViewport::SetCamera(const CameraPtr& camera)
{
	m_camera = camera;
}

SceneNodePtr CViewport::GetSceneRoot() const
{
	return m_sceneRoot;
}

CameraPtr CViewport::GetShadowCamera() const
{
	return m_shadowCamera;
}

void CViewport::SetShadowCamera(const CameraPtr& shadowCamera)
{
	m_shadowCamera = shadowCamera;
}
