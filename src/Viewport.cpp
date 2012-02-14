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
	return std::make_shared<CViewport>();
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
