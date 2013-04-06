#include "athena/win32/Dx11Texture.h"
#include <assert.h>
#include "Bitmap.h"
#include "PNG.h"
#include "StdStream.h"

using namespace Athena;

static const DXGI_FORMAT g_textureFormats[TEXTURE_FORMAT_MAX] =
{
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM,
};

CDx11Texture::CDx11Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11Texture2D* texture)
: m_device(device)
, m_deviceContext(deviceContext)
, m_texture(texture)
, m_textureView(nullptr)
{
	HRESULT result = device->CreateShaderResourceView(texture, nullptr, &m_textureView);
	assert(SUCCEEDED(result));
}

CDx11Texture::~CDx11Texture()
{
	if(m_texture)
	{
		m_texture->Release();
	}
	if(m_textureView)
	{
		m_textureView->Release();
	}
}

TexturePtr CDx11Texture::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	auto specTextureFormat = g_textureFormats[textureFormat];

	ID3D11Texture2D* texture(nullptr);

	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= width;
		textureDesc.Height				= height;
		textureDesc.MipLevels			= 1;
		textureDesc.ArraySize			= 1;
		textureDesc.Format				= specTextureFormat;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
		textureDesc.Usage				= D3D11_USAGE_DYNAMIC;
		textureDesc.SampleDesc.Count	= 1;
		textureDesc.SampleDesc.Quality	= 0;
		HRESULT result = device->CreateTexture2D(&textureDesc, nullptr, &texture);
		assert(SUCCEEDED(result));
	}

	auto result = std::make_shared<CDx11Texture>(device, deviceContext, texture);
	result->m_format = textureFormat;
	result->m_width = width;
	result->m_height = height;
	return result;
}

TexturePtr CDx11Texture::CreateFromFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* path)
{
	Framework::CBitmap imageData = Framework::CPNG::ReadBitmap(Framework::CStdStream(path, "rb"));
	if(imageData.GetBitsPerPixel() == 24)
	{
		imageData = imageData.AddAlphaChannel(0xFF);
	}

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
		textureDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureDesc.SampleDesc.Count	= 1;
		textureDesc.SampleDesc.Quality	= 0;
		HRESULT result = device->CreateTexture2D(&textureDesc, &subResourceData, &texture);
		assert(SUCCEEDED(result));
	}

	return std::make_shared<CDx11Texture>(device, deviceContext, texture);
}

TexturePtr CDx11Texture::CreateFromMemory(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const void* data, uint32 dataSize)
{
	ID3D11Texture2D* texture(nullptr);
//	HRESULT result = D3DXCreateTextureFromFileInMemory(device, data, dataSize, &texture);
//	assert(SUCCEEDED(result));
	return std::make_shared<CDx11Texture>(device, deviceContext, texture);
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

ID3D11ShaderResourceView* CDx11Texture::GetTextureView() const
{
	return m_textureView;
}

void CDx11Texture::Update(const void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	assert(m_format == TEXTURE_FORMAT_RGBA8888);
	uint32 srcPitch = c_textureFormatSize[m_format] * m_width;

	const uint8* srcPtr = reinterpret_cast<const uint8*>(data);
	uint8* dstPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	for(uint32 y = 0; y < m_height; y++)
	{
		memcpy(dstPtr, srcPtr, srcPitch);
		srcPtr += srcPitch;
		dstPtr += mappedResource.RowPitch;
	}

	m_deviceContext->Unmap(m_texture, 0);
}
