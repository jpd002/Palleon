#include "athena/win32/Dx11Texture.h"
#include "athena/resources/DdsImage.h"
#include <assert.h>
#include <algorithm>
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
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC3_UNORM
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

TexturePtr CDx11Texture::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height, uint32 mipCount)
{
	assert(mipCount > 0);

	auto specTextureFormat = g_textureFormats[textureFormat];

	ID3D11Texture2D* texture(nullptr);

	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= width;
		textureDesc.Height				= height;
		textureDesc.MipLevels			= mipCount;
		textureDesc.ArraySize			= 1;
		textureDesc.Format				= specTextureFormat;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Usage				= D3D11_USAGE_DEFAULT;
		textureDesc.SampleDesc.Count	= 1;
		textureDesc.SampleDesc.Quality	= 0;
		HRESULT result = device->CreateTexture2D(&textureDesc, nullptr, &texture);
		assert(SUCCEEDED(result));
	}

	auto result = std::make_shared<CDx11Texture>(device, deviceContext, texture);
	result->m_format = textureFormat;
	result->m_width = width;
	result->m_height = height;
	result->m_mipCount = mipCount;
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

TexturePtr CDx11Texture::CreateCube(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TEXTURE_FORMAT textureFormat, uint32 size)
{
	auto specTextureFormat = g_textureFormats[textureFormat];

	ID3D11Texture2D* texture(nullptr);

	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= size;
		textureDesc.Height				= size;
		textureDesc.MipLevels			= 1;
		textureDesc.ArraySize			= 6;
		textureDesc.Format				= specTextureFormat;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Usage				= D3D11_USAGE_DEFAULT;
		textureDesc.MiscFlags			= D3D11_RESOURCE_MISC_TEXTURECUBE;
		textureDesc.SampleDesc.Count	= 1;
		textureDesc.SampleDesc.Quality	= 0;
		HRESULT result = device->CreateTexture2D(&textureDesc, nullptr, &texture);
		assert(SUCCEEDED(result));
	}

	auto result = std::make_shared<CDx11Texture>(device, deviceContext, texture);
	result->m_format = textureFormat;
	result->m_width = size;
	result->m_height = size;
	result->m_mipCount = 1;
	result->m_isCube = true;
	return result;
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

void CDx11Texture::Update(uint32 mipLevel, const void* data)
{
	assert(!m_isCube);
	assert(mipLevel < m_mipCount);
	uint32 actualWidth = std::max<uint32>(m_width >> mipLevel, 1);
	uint32 actualHeight = std::max<uint32>(m_height >> mipLevel, 1);
	auto subresourceIndex = D3D11CalcSubresource(mipLevel, 0, m_mipCount);
	UpdateSurface(subresourceIndex, actualWidth, actualHeight, data);
}

void CDx11Texture::UpdateCubeFace(TEXTURE_CUBE_FACE face, const void* data)
{
	static const D3D11_TEXTURECUBE_FACE c_cubeFaces[TEXTURE_CUBE_FACE_MAX] =
	{
		D3D11_TEXTURECUBE_FACE_POSITIVE_X,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_X,
		D3D11_TEXTURECUBE_FACE_POSITIVE_Y,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_Y,
		D3D11_TEXTURECUBE_FACE_POSITIVE_Z,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_Z
	};

	assert(m_mipCount != 0);
	assert(m_isCube);
	auto subresourceIndex = D3D11CalcSubresource(0, c_cubeFaces[face], m_mipCount);
	UpdateSurface(subresourceIndex, m_width, m_height, data);
}

void CDx11Texture::UpdateSurface(unsigned int subresourceIndex, unsigned int width, unsigned int height, const void* data)
{
	auto srcPitches = GetTexturePitches(m_format, width, height);

	if(m_format == TEXTURE_FORMAT_RGB888)
	{
		//Conversion needed
		const uint32 cvtPitch = m_width * 4;
		const uint32 cvtDepthPitch = cvtPitch * m_height;
		std::vector<uint8> conversionBuffer(cvtDepthPitch);
		const uint8* srcPtr = reinterpret_cast<const uint8*>(data);
		uint8* dstPtr = conversionBuffer.data();
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
			srcPtr += srcPitches.first;
			dstPtr += cvtPitch;
		}
		m_deviceContext->UpdateSubresource(m_texture, subresourceIndex, nullptr, conversionBuffer.data(), cvtPitch, cvtDepthPitch);
	}
	else
	{
		m_deviceContext->UpdateSubresource(m_texture, subresourceIndex, nullptr, data, srcPitches.first, srcPitches.second);
	}
}

CDx11Texture::PitchPair CDx11Texture::GetTexturePitches(TEXTURE_FORMAT format, unsigned int width, unsigned int height)
{
	if(format == TEXTURE_FORMAT_DXT1 || format == TEXTURE_FORMAT_DXT3 || format == TEXTURE_FORMAT_DXT5)
	{
		uint32 blockSize = c_textureFormatSize[format];
		uint32 srcPitch = max(1, (width + 3) / 4) * blockSize;
		uint32 srcDepthPitch = srcPitch * ((height + 3) / 4);
		return std::make_pair(srcPitch, srcDepthPitch);
	}
	else
	{
		uint32 srcPitch = (c_textureFormatSize[format] * width) / 8;
		uint32 srcDepthPitch = srcPitch * height;
		return std::make_pair(srcPitch, srcDepthPitch);
	}
}
