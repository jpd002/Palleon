#include "athena/win32/Dx11Texture.h"
#include <assert.h>
#include "Bitmap.h"
#include "PNG.h"
#include "StdStream.h"

using namespace Athena;

static const DXGI_FORMAT g_textureFormats[TEXTURE_FORMAT_MAX] =
{
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_UINT,
};

CDx11Texture::CDx11Texture(ID3D11Texture2D* texture)
: m_texture(texture)
{

}

CDx11Texture::~CDx11Texture()
{
	if(m_texture)
	{
		m_texture->Release();
	}
}

TexturePtr CDx11Texture::Create(ID3D11Device* device, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	auto specTextureFormat = g_textureFormats[textureFormat];

	ID3D11Texture2D* texture(nullptr);

	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width		= width;
		textureDesc.Height		= height;
		textureDesc.Format		= specTextureFormat;
		HRESULT result = device->CreateTexture2D(&textureDesc, nullptr, &texture);
		assert(SUCCEEDED(result));
	}

	auto result = std::make_shared<CDx11Texture>(texture);
	result->m_format = textureFormat;
	result->m_width = width;
	result->m_height = height;
	return result;
}

TexturePtr CDx11Texture::CreateFromFile(ID3D11Device* device, const char* path)
{
	Framework::CBitmap imageData = Framework::CPNG::ReadBitmap(Framework::CStdStream(path, "rb"));

	ID3D11Texture2D* texture(nullptr);

	{
		D3D11_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pSysMem				= imageData.GetPixels();
		subResourceData.SysMemPitch			= imageData.GetPitch();
		subResourceData.SysMemSlicePitch	= 0;

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= imageData.GetWidth();
		textureDesc.Height				= imageData.GetHeight();
		textureDesc.MipLevels			= 1;
		textureDesc.ArraySize			= 1;
		textureDesc.Format				= DXGI_FORMAT_R8G8B8A8_UINT;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureDesc.SampleDesc.Count	= 1;
		textureDesc.SampleDesc.Quality	= 0;
		HRESULT result = device->CreateTexture2D(&textureDesc, &subResourceData, &texture);
		assert(SUCCEEDED(result));
	}

	return std::make_shared<CDx11Texture>(texture);
}

TexturePtr CDx11Texture::CreateFromMemory(ID3D11Device* device, const void* data, uint32 dataSize)
{
	ID3D11Texture2D* texture(nullptr);
//	HRESULT result = D3DXCreateTextureFromFileInMemory(device, data, dataSize, &texture);
//	assert(SUCCEEDED(result));
	return std::make_shared<CDx11Texture>(texture);
}

TexturePtr CDx11Texture::CreateCubeFromFile(ID3D11Device* device, const char* path)
{
//	IDirect3DCubeTexture9* texture(NULL);
//	HRESULT result = D3DXCreateCubeTextureFromFileA(device, path, &texture);
//	assert(SUCCEEDED(result));
//	return std::make_shared<CDx11Texture>(texture);
	return TexturePtr();
}

void* CDx11Texture::GetHandle() const
{
	return m_texture;
}

void CDx11Texture::Update(const void* data)
{
/*
	assert(m_format != TEXTURE_FORMAT_UNKNOWN);
	uint32 srcPitch = c_textureFormatSize[m_format] * m_width;
	auto texture = static_cast<IDirect3DTexture9*>(m_texture);

	D3DLOCKED_RECT lockedRect;
	texture->LockRect(0, &lockedRect, NULL, 0);
	const uint8* srcPtr = reinterpret_cast<const uint8*>(data);
	uint8* dstPtr = reinterpret_cast<uint8*>(lockedRect.pBits);
	for(uint32 y = 0; y < m_height; y++)
	{
		memcpy(dstPtr, srcPtr, srcPitch);
		srcPtr += srcPitch;
		dstPtr += lockedRect.Pitch;
	}
	texture->UnlockRect(0);
*/
}
