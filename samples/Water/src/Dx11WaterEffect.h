#pragma once

#include "palleon/win32/Dx11Effect.h"

class CDx11WaterEffect : public Palleon::CDx11Effect
{
public:
							CDx11WaterEffect(ID3D11Device*, ID3D11DeviceContext*);
	virtual					~CDx11WaterEffect();

	void					UpdateConstants(const Palleon::DX11VIEWPORT_PARAMS&, Palleon::CMaterial*, const CMatrix4&) override;

private:
	D3D11InputLayoutPtr		CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR&) override;

	uint32					m_worldMatrixOffset = -1;
	uint32					m_viewProjMatrixOffset = -1;
	uint32					m_texture1MatrixOffset = -1;
	uint32					m_cameraPositionOffset = -1;
};
