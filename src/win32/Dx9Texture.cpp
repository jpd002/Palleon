#include "athena/win32/Dx9Texture.h"
#include <assert.h>
#include <d3dx9.h>

using namespace Athena;

CDx9Texture::CDx9Texture(IDirect3DDevice9* device, const char* path)
: m_texture(NULL)
{
	HRESULT result = D3DXCreateTextureFromFileA(device, path, &m_texture);
	assert(SUCCEEDED(result));
}

CDx9Texture::CDx9Texture(IDirect3DDevice9* device, const void* data, uint32 dataSize)
: m_texture(NULL)
{
	HRESULT result = D3DXCreateTextureFromFileInMemory(device, data, dataSize, &m_texture);
	assert(SUCCEEDED(result));
}

CDx9Texture::CDx9Texture(IDirect3DDevice9* device, const void* data, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	D3DFORMAT specTextureFormat = D3DFMT_R8G8B8;
	uint32 srcPitch = 3 * width;
	switch(textureFormat)
	{
	case TEXTURE_FORMAT_RGB888:
		specTextureFormat = D3DFMT_R8G8B8;
		srcPitch = 3 * width;
		break;
	default:
		assert(0);
		break;
	}
	HRESULT result = D3DXCreateTexture(device, width, height, 0, 0, specTextureFormat, D3DPOOL_MANAGED, &m_texture);
	assert(SUCCEEDED(result));

	{
		D3DLOCKED_RECT lockedRect;
		m_texture->LockRect(0, &lockedRect, NULL, 0);
		const uint8* srcPtr = reinterpret_cast<const uint8*>(data);
		uint8* dstPtr = reinterpret_cast<uint8*>(lockedRect.pBits);
		for(uint32 y = 0; y < height; y++)
		{
			for(uint32 x = 0; x < width; x++)
			{
				dstPtr[(x * 4) + 0] = srcPtr[(x * 3) + 2];
				dstPtr[(x * 4) + 1] = srcPtr[(x * 3) + 1];
				dstPtr[(x * 4) + 2] = srcPtr[(x * 3) + 0];
				dstPtr[(x * 4) + 3] = 0xFF;
			}
			srcPtr += srcPitch;
			dstPtr += lockedRect.Pitch;
		}
		m_texture->UnlockRect(0);
	}
}

CDx9Texture::~CDx9Texture()
{
	if(m_texture != NULL)
	{
		m_texture->Release();
		m_texture = NULL;
	}
}

IDirect3DTexture9* CDx9Texture::GetTexture() const
{
	return m_texture;
}
