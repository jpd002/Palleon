#include "athena/win32/Dx9Texture.h"
#include <assert.h>
#include <d3dx9.h>

using namespace Athena;

static const D3DFORMAT g_textureFormats[TEXTURE_FORMAT_MAX] =
{
	D3DFMT_UNKNOWN,
	D3DFMT_R8G8B8,
	D3DFMT_A8R8G8B8
};

CDx9Texture::CDx9Texture(IDirect3DBaseTexture9* texture)
: m_texture(texture)
{

}

CDx9Texture::~CDx9Texture()
{
	if(m_texture != NULL)
	{
		m_texture->Release();
		m_texture = NULL;
	}
}

TexturePtr CDx9Texture::Create(IDirect3DDevice9* device, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	D3DFORMAT specTextureFormat = g_textureFormats[textureFormat];
	IDirect3DTexture9* texture(nullptr);
	{
		HRESULT result = D3DXCreateTexture(device, width, height, 0, 0, specTextureFormat, D3DPOOL_MANAGED, &texture);
		assert(SUCCEEDED(result));
	}

	auto result = std::make_shared<CDx9Texture>(texture);
	result->m_format = textureFormat;
	result->m_width = width;
	result->m_height = height;
	return result;
}

TexturePtr CDx9Texture::CreateFromFile(IDirect3DDevice9* device, const char* path)
{
	IDirect3DTexture9* texture(NULL);
	HRESULT result = D3DXCreateTextureFromFileA(device, path, &texture);
	assert(SUCCEEDED(result));
	return std::make_shared<CDx9Texture>(texture);
}

TexturePtr CDx9Texture::CreateFromMemory(IDirect3DDevice9* device, const void* data, uint32 dataSize)
{
	IDirect3DTexture9* texture(NULL);
	HRESULT result = D3DXCreateTextureFromFileInMemory(device, data, dataSize, &texture);
	assert(SUCCEEDED(result));
	return std::make_shared<CDx9Texture>(texture);
}

TexturePtr CDx9Texture::CreateCubeFromFile(IDirect3DDevice9* device, const char* path)
{
	IDirect3DCubeTexture9* texture(NULL);
	HRESULT result = D3DXCreateCubeTextureFromFileA(device, path, &texture);
	assert(SUCCEEDED(result));
	return std::make_shared<CDx9Texture>(texture);
}

void* CDx9Texture::GetHandle() const
{
	return m_texture;
}

void CDx9Texture::Update(const void* data)
{
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
}
