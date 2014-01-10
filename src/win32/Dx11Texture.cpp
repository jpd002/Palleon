#include "athena/win32/Dx11Texture.h"
#include "athena/resources/DdsImage.h"
#include <assert.h>
#include "bitmap/Bitmap.h"
#include "bitmap/PNG.h"
#include "bitmap/JPEG.h"
#include "bitmap/TGA.h"
#include "StdStream.h"
#include "PtrStream.h"

using namespace Athena;

static const DXGI_FORMAT g_textureFormats[TEXTURE_FORMAT_MAX] =
{
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_BC1_UNORM,
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
	return CreateFromStream(device, deviceContext, Framework::CStdStream(path, "rb"));
}

TexturePtr CDx11Texture::CreateFromMemory(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const void* data, uint32 dataSize)
{
	return CreateFromStream(device, deviceContext, Framework::CPtrStream(data, dataSize));
}

TexturePtr CDx11Texture::CreateCubeFromFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* path)
{
	return CreateCubeFromStream(device, deviceContext, Framework::CStdStream(path, "rb"));
}

TexturePtr CDx11Texture::CreateFromStream(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Framework::CStream& stream)
{
	Framework::CBitmap imageData;

	try
	{
		uint8 header[16];
		stream.Read(header, 16);
		stream.Seek(0, Framework::STREAM_SEEK_SET);

		if((header[0] == 0x89) && (header[1] == 'P') && (header[2] == 'N') && (header[3] == 'G'))
		{
			imageData = Framework::CPNG::ReadBitmap(stream);
		}
		else if((header[0] == 0xFF) && (header[1] == 0xD8))
		{
			imageData = Framework::CJPEG::ReadBitmap(stream);
		}
		else if(
				(header[0] == 0x00) && (header[1] == 0x00) && (header[2] == 0x02) && (header[3] == 0x00) &&
				(header[4] == 0x00) && (header[5] == 0x00) && (header[6] == 0x00) && (header[7] == 0x00)
			)
		{
			imageData = Framework::CTGA::ReadBitmap(stream);
		}
		else
		{
			throw std::exception();
		}
	}
	catch(...)
	{
		return TexturePtr();
	}

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

TexturePtr CDx11Texture::CreateCubeFromStream(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Framework::CStream& stream)
{
	CDdsImage image(stream);
	assert(image.IsCubeMap());
	if(!image.IsCubeMap()) return TexturePtr();

	const auto& surfaces(image.GetSurfaces());

	ID3D11Texture2D* texture(nullptr);

	{
		D3D11_SUBRESOURCE_DATA subResourceData[6];
		memset(subResourceData, 0, sizeof(subResourceData));
		for(unsigned int i = 0; i < 6; i++)
		{
			auto& faceSubResourceData(subResourceData[i]);
			faceSubResourceData.pSysMem				= surfaces[i].data();
			faceSubResourceData.SysMemPitch			= image.GetPitch();
			faceSubResourceData.SysMemSlicePitch	= 0;
		}

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= image.GetWidth();
		textureDesc.Height				= image.GetHeight();
		textureDesc.MipLevels			= 1;
		textureDesc.ArraySize			= 6;
		textureDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags			= D3D11_RESOURCE_MISC_TEXTURECUBE;
		textureDesc.SampleDesc.Count	= 1;
		textureDesc.SampleDesc.Quality	= 0;

		HRESULT result = device->CreateTexture2D(&textureDesc, subResourceData, &texture);
		assert(SUCCEEDED(result));
	}

	return std::make_shared<CDx11Texture>(device, deviceContext, texture);
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

	uint32 srcPitch = (c_textureFormatSize[m_format] * m_width) / 8;
	const uint8* srcPtr = reinterpret_cast<const uint8*>(data);
	uint8* dstPtr = reinterpret_cast<uint8*>(mappedResource.pData);

	if(m_format == TEXTURE_FORMAT_RGB888)
	{
		//Conversion needed
		for(uint32 y = 0; y < m_height; y++)
		{
			const uint8* srcLinePtr = srcPtr;
			uint8* dstLinePtr = dstPtr;
			for(uint32 x = 0; x < m_width; x++)
			{
				uint8 r = srcLinePtr[0];
				uint8 g = srcLinePtr[1];
				uint8 b = srcLinePtr[2];
				dstLinePtr[0] = r;
				dstLinePtr[1] = g;
				dstLinePtr[2] = b;
				dstLinePtr[3] = 0xFF;
				srcLinePtr += 3;
				dstLinePtr += 4;
			}
			srcPtr += srcPitch;
			dstPtr += mappedResource.RowPitch;
		}
	}
	else if(m_format == TEXTURE_FORMAT_DXT1)
	{
		assert(mappedResource.DepthPitch == (m_width * m_height * 4 / 8));
		memcpy(dstPtr, data, mappedResource.DepthPitch);
	}
	else
	{
		for(uint32 y = 0; y < m_height; y++)
		{
			memcpy(dstPtr, srcPtr, srcPitch);
			srcPtr += srcPitch;
			dstPtr += mappedResource.RowPitch;
		}
	}

	m_deviceContext->Unmap(m_texture, 0);
}
