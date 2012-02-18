#include "athena/win32/Dx9RenderTarget.h"
#include "athena/win32/Dx9GraphicDevice.h"
#include <d3dx9.h>

using namespace Athena;

CDx9RenderTarget::CDx9RenderTarget(IDirect3DDevice9* device, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	D3DFORMAT specTextureFormat = CDx9GraphicDevice::ConvertTextureFormatId(textureFormat);
	HRESULT result = D3DXCreateTexture(device, width, height, 1, D3DUSAGE_RENDERTARGET, specTextureFormat, D3DPOOL_DEFAULT, &m_texture);
	assert(SUCCEEDED(result));
}

CDx9RenderTarget::~CDx9RenderTarget()
{
	m_texture->Release();
}

void CDx9RenderTarget::Draw(const ViewportPtr& viewport)
{
	CDx9GraphicDevice& graphicDevice = static_cast<CDx9GraphicDevice&>(CGraphicDevice::GetInstance());

	IDirect3DSurface9* renderSurface(NULL);
	HRESULT result = m_texture->GetSurfaceLevel(0, &renderSurface);
	assert(SUCCEEDED(result));

	graphicDevice.DrawViewportToSurface(renderSurface, viewport.get());

	renderSurface->Release();
}

void* CDx9RenderTarget::GetHandle() const
{
	return m_texture;
}
