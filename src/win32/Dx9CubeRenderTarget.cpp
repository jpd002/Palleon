#include "athena/win32/Dx9CubeRenderTarget.h"
#include "athena/win32/Dx9GraphicDevice.h"
#include <d3dx9.h>

using namespace Athena;

static const D3DCUBEMAP_FACES g_cubeMapFaces[CUBEMAP_FACE_MAX] =
{
	D3DCUBEMAP_FACE_POSITIVE_X,
	D3DCUBEMAP_FACE_NEGATIVE_X,

	D3DCUBEMAP_FACE_POSITIVE_Y,
	D3DCUBEMAP_FACE_NEGATIVE_Y,

	D3DCUBEMAP_FACE_POSITIVE_Z,
	D3DCUBEMAP_FACE_NEGATIVE_Z
};

CDx9CubeRenderTarget::CDx9CubeRenderTarget(IDirect3DDevice9* device, TEXTURE_FORMAT textureFormat, uint32 size)
{
	D3DFORMAT specTextureFormat = CDx9GraphicDevice::ConvertTextureFormatId(textureFormat);
	HRESULT result = D3DXCreateCubeTexture(device, size, 1, D3DUSAGE_RENDERTARGET, specTextureFormat, D3DPOOL_DEFAULT, &m_texture);
	assert(SUCCEEDED(result));
}

CDx9CubeRenderTarget::~CDx9CubeRenderTarget()
{
	m_texture->Release();
}

void CDx9CubeRenderTarget::Draw(CUBEMAP_FACE face, const ViewportPtr& viewport)
{
	CDx9GraphicDevice& graphicDevice = static_cast<CDx9GraphicDevice&>(CGraphicDevice::GetInstance());

	IDirect3DSurface9* renderSurface(NULL);
	HRESULT result = m_texture->GetCubeMapSurface(g_cubeMapFaces[face], 0, &renderSurface);
	assert(SUCCEEDED(result));

	graphicDevice.DrawViewportToSurface(renderSurface, viewport.get());

	renderSurface->Release();
}

void* CDx9CubeRenderTarget::GetHandle() const
{
	return m_texture;
}
