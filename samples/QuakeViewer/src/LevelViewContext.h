#ifndef _LEVELVIEWCONTEXT_H_
#define _LEVELVIEWCONTEXT_H_

#include "ContextBase.h"
#include "PakFile.h"
#include "BspFile.h"
#include "BspMapResourceProvider.h"
#include "BspMapMeshProvider.h"

class CLevelViewContext : public CContextBase
{
public:
									CLevelViewContext(CPakFile*, const char*);
	virtual							~CLevelViewContext();

	virtual void					Update(float) override;

	virtual void					NotifyMouseMove(int, int) override;
	virtual void					NotifyMouseDown() override;
	virtual void					NotifyMouseUp() override;

private:
	enum COMMAND_MODE
	{
		COMMAND_MODE_IDLE			= 0,
		COMMAND_MODE_DRAG_CAMERA	= 1,
		COMMAND_MODE_MOVE_FORWARD	= 2,
		COMMAND_MODE_MOVE_BACKWARD	= 3,
	};

	void							InitializeMapViewport();
	void							InitializeHudViewport();
	void							InitializeReflectionMap();

	void							UpdateCamera();
	void							UpdateReflectionMap();

	Palleon::PackagePtr				m_viewerPackage;

	Palleon::ViewportPtr			m_mapViewport;
	Palleon::CameraPtr				m_mapCamera;

	Palleon::ViewportPtr			m_hudViewport;
	Palleon::LabelPtr				m_positionLabel;
	Palleon::LabelPtr				m_metricsLabel;
	Palleon::SpritePtr				m_renderTargetSprite;

	Palleon::TexturePtr				m_skyTexture;

	Palleon::CubeRenderTargetPtr	m_reflectionRenderTarget;
	Palleon::CameraPtr				m_reflectionCamera;

	Palleon::MeshPtr				m_sphere;
	CVector3						m_sphereBasePosition;

	COMMAND_MODE					m_commandMode;

	CVector2						m_mousePosition;
	CVector3						m_cameraPosition;

	CBspFile*						m_bspFile;
	BspMapMeshProviderPtr			m_bspMapMeshProvider;
	CBspMapResourceProvider*		m_bspMapResourceProvider;

	float							m_cameraHAngle;
	float							m_dragHAngle;

	float							m_cameraVAngle;
	float							m_dragVAngle;

	CVector2						m_dragPosition;

	float							m_elapsed;

	CBox2							m_forwardButtonBoundingBox;
	CBox2							m_backwardButtonBoundingBox;
};

#endif
