#include "athena/win32/Dx9Texture.h"
#include <assert.h>
#include <d3dx9.h>

using namespace Athena;

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

TexturePtr CDx9Texture::CreateFromFile(IDirect3DDevice9* device, const char* path)
{
	IDirect3DTexture9* texture(NULL);
	HRESULT result = D3DXCreateTextureFromFileA(device, path, &texture);
	assert(SUCCEEDED(result));
	return TexturePtr(new CDx9Texture(texture));
}

TexturePtr CDx9Texture::CreateFromMemory(IDirect3DDevice9* device, const void* data, uint32 dataSize)
{
	IDirect3DTexture9* texture(NULL);
	HRESULT result = D3DXCreateTextureFromFileInMemory(device, data, dataSize, &texture);
	assert(SUCCEEDED(result));
	return TexturePtr(new CDx9Texture(texture));
}

TexturePtr CDx9Texture::CreateFromRawData(IDirect3DDevice9* device, const void* data, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
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

	IDirect3DTexture9* texture(NULL);
	HRESULT result = D3DXCreateTexture(device, width, height, 0, 0, specTextureFormat, D3DPOOL_MANAGED, &texture);
	assert(SUCCEEDED(result));

	{
		D3DLOCKED_RECT lockedRect;
		texture->LockRect(0, &lockedRect, NULL, 0);
		const uint8* srcPtr = reinterpret_cast<const uint8*>(data);
		uint8* dstPtr = reinterpret_cast<uint8*>(lockedRect.pBits);
		for(uint32 y = 0; y < height; y++)
		{
			for(uint32 x = 0; x < width; x++)
			{
				dstPtr[(x * 4) + 0] = std::min<uint32>(0xFF, static_cast<uint32>(srcPtr[(x * 3) + 2] * 2));
				dstPtr[(x * 4) + 1] = std::min<uint32>(0xFF, static_cast<uint32>(srcPtr[(x * 3) + 1] * 2));
				dstPtr[(x * 4) + 2] = std::min<uint32>(0xFF, static_cast<uint32>(srcPtr[(x * 3) + 0] * 2));
				dstPtr[(x * 4) + 3] = 0xFF;
			}
			srcPtr += srcPitch;
			dstPtr += lockedRect.Pitch;
		}
		texture->UnlockRect(0);
	}

	return TexturePtr(new CDx9Texture(texture));
}

TexturePtr CDx9Texture::CreateCubeFromFile(IDirect3DDevice9* device, const char* path)
{
	IDirect3DCubeTexture9* texture(NULL);
	HRESULT result = D3DXCreateCubeTextureFromFileA(device, path, &texture);
	assert(SUCCEEDED(result));
	return TexturePtr(new CDx9Texture(texture));
}

IDirect3DBaseTexture9* CDx9Texture::GetTexture() const
{
	return m_texture;
}
