#include <assert.h>
#include <algorithm>
#include "athena/GraphicDevice.h"
#include "athena/Vector3.h"
#include "athena/Vector2.h"
#include "bitmap/Bitmap.h"
#include "bitmap/PNG.h"
#include "bitmap/JPEG.h"
#include "bitmap/TGA.h"
#include "athena/resources/PvrImage.h"
#include "StdStream.h"
#include "PtrStream.h"

using namespace Athena;

CGraphicDevice* CGraphicDevice::m_instance = NULL;

CGraphicDevice::CGraphicDevice()
: m_screenSize(0, 0)
, m_drawCallCount(0)
, m_frameRate(0)
{

}

CGraphicDevice::~CGraphicDevice()
{

}

CGraphicDevice& CGraphicDevice::GetInstance()
{
	assert(m_instance != NULL);
	return (*m_instance);
}

CVector2 CGraphicDevice::GetScreenSize() const
{
	return m_screenSize;
}

uint32 CGraphicDevice::GetDrawCallCount() const
{
	return m_drawCallCount;
}

float CGraphicDevice::GetFrameRate() const
{
	return m_frameRate;
}

EffectProviderPtr CGraphicDevice::GetDefaultEffectProvider() const
{
	return m_defaultEffectProvider;
}

TexturePtr CGraphicDevice::CreateTextureFromStream(Framework::CStream& stream)
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
	
	auto texture = CreateTexture(Athena::TEXTURE_FORMAT_RGBA8888, imageData.GetWidth(), imageData.GetHeight(), 1);
	texture->Update(0, imageData.GetPixels());
	return texture;
}

TexturePtr CGraphicDevice::CreateTextureFromFile(const std::string& path)
{
	Framework::CStdStream stream(path.c_str(), "rb");
	return CreateTextureFromStream(stream);
}

TexturePtr CGraphicDevice::CreateTextureFromMemory(const void* data, uint32 dataSize)
{
	Framework::CPtrStream stream(data, dataSize);
	return CreateTextureFromStream(stream);
}

TexturePtr CGraphicDevice::CreateCubeTextureFromStream(Framework::CStream& stream)
{
	std::vector<uint8> texData(stream.GetLength());
	stream.Read(texData.data(), texData.size());
	const uint8* pvrFile = reinterpret_cast<const uint8*>(texData.data());
	uint32 pvrFileSize = texData.size();
	
	if(pvrFileSize < sizeof(PVR_HEADER))
	{
		assert(0);
		return 0;
	}
	
	const PVR_HEADER* header = reinterpret_cast<const PVR_HEADER*>(pvrFile);
	if(!header->IsValid())
	{
		return 0;
	}
	
	uint8 pixelFormat = header->GetPixelFormat();
	if(pixelFormat != PVR_PIXEL_OGL_PVRTC4)
	{
		return 0;
	}
	
	if((header->flags & PVR_FLAG_CUBEMAP) == 0)
	{
		return 0;
	}
	
	if(header->surfaceCount != 6)
	{
		return 0;
	}
	
	static const TEXTURE_CUBE_FACE c_cubeMapIndices[6] =
	{
		TEXTURE_CUBE_FACE_POSITIVE_X,
		TEXTURE_CUBE_FACE_NEGATIVE_X,
		TEXTURE_CUBE_FACE_NEGATIVE_Y,
		TEXTURE_CUBE_FACE_POSITIVE_Y,
		TEXTURE_CUBE_FACE_POSITIVE_Z,
		TEXTURE_CUBE_FACE_NEGATIVE_Z,
	};
	
	auto texture = CreateCubeTexture(TEXTURE_FORMAT_PVRTC4, header->width);
		
	const uint8* surfaceDataPtr = pvrFile + sizeof(PVR_HEADER);
	for(unsigned int i = 0; i < 6; i++)
	{
		texture->UpdateCubeFace(c_cubeMapIndices[i], surfaceDataPtr);
		surfaceDataPtr += header->dataSize;
	}
	
	return texture;
}

TexturePtr CGraphicDevice::CreateCubeTextureFromFile(const std::string& path)
{
	Framework::CStdStream stream(path.c_str(), "rb");
	return CreateCubeTextureFromStream(stream);
}

void CGraphicDevice::AddViewport(CViewport* viewport)
{
	assert(std::find(std::begin(m_viewports), std::end(m_viewports), viewport) == std::end(m_viewports));
	m_viewports.push_back(viewport);
}

void CGraphicDevice::AddViewportAfter(CViewport* reference, CViewport* viewport)
{
	assert(std::find(std::begin(m_viewports), std::end(m_viewports), viewport) == std::end(m_viewports));

	auto viewportIterator(std::find(std::begin(m_viewports), std::end(m_viewports), reference));
	assert(viewportIterator != m_viewports.end());

	if(viewportIterator == m_viewports.end()) return;
	std::advance(viewportIterator, 1);
	m_viewports.insert(viewportIterator, viewport);
}

void CGraphicDevice::RemoveViewport(CViewport* viewport)
{
	auto viewportIterator = std::find(std::begin(m_viewports), std::end(m_viewports), viewport);
	assert(viewportIterator != std::end(m_viewports));
	m_viewports.erase(viewportIterator);
}
