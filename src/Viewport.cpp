#include "athena/Viewport.h"

using namespace Athena;

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
