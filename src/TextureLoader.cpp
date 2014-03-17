#include "athena/TextureLoader.h"
#include "athena/GraphicDevice.h"
#include "athena/resources/PvrImage.h"
#include "athena/resources/DdsImage.h"
#include "bitmap/Bitmap.h"
#include "bitmap/PNG.h"
#include "bitmap/JPEG.h"
#include "bitmap/TGA.h"
#include "StdStream.h"
#include "PtrStream.h"

using namespace Athena;

TexturePtr CTextureLoader::CreateTextureFromStream(Framework::CStream& stream)
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
	
	auto texture = CGraphicDevice::GetInstance().CreateTexture(Athena::TEXTURE_FORMAT_RGBA8888, imageData.GetWidth(), imageData.GetHeight(), 1);
	texture->Update(0, imageData.GetPixels());
	return texture;
}

TexturePtr CTextureLoader::CreateTextureFromFile(const std::string& path)
{
	Framework::CStdStream stream(path.c_str(), "rb");
	return CreateTextureFromStream(stream);
}

TexturePtr CTextureLoader::CreateTextureFromMemory(const void* data, uint32 dataSize)
{
	Framework::CPtrStream stream(data, dataSize);
	return CreateTextureFromStream(stream);
}

TexturePtr CTextureLoader::CreateCubeTextureFromStream(Framework::CStream& stream)
{
	try
	{
		uint8 header[16];
		stream.Read(header, 16);
		stream.Seek(0, Framework::STREAM_SEEK_SET);

		if(header[0] == 'D' && header[1] == 'D' && header[2] == 'S' && header[3] == ' ')
		{
			return CreateCubeTextureFromStreamDds(stream);
		}
		else
		{
			return CreateCubeTextureFromStreamPvr(stream);
		}
	}
	catch(...)
	{
		return TexturePtr();
	}
}

TexturePtr CTextureLoader::CreateCubeTextureFromFile(const std::string& path)
{
	Framework::CStdStream stream(path.c_str(), "rb");
	return CreateCubeTextureFromStream(stream);
}

TexturePtr CTextureLoader::CreateCubeTextureFromStreamDds(Framework::CStream& stream)
{
	CDdsImage image(stream);
	assert(image.IsCubeMap());
	if(!image.IsCubeMap()) return TexturePtr();

	//We need a better way to determine the texture format here
	auto texture = CGraphicDevice::GetInstance().CreateCubeTexture(TEXTURE_FORMAT_BGRA8888, image.GetWidth());

	const auto& surfaces(image.GetSurfaces());

	static const TEXTURE_CUBE_FACE c_cubeMapIndices[6] =
	{
		TEXTURE_CUBE_FACE_POSITIVE_X,
		TEXTURE_CUBE_FACE_NEGATIVE_X,
		TEXTURE_CUBE_FACE_POSITIVE_Y,
		TEXTURE_CUBE_FACE_NEGATIVE_Y,
		TEXTURE_CUBE_FACE_POSITIVE_Z,
		TEXTURE_CUBE_FACE_NEGATIVE_Z,
	};

	for(unsigned int i = 0; i < 6; i++)
	{
		texture->UpdateCubeFace(c_cubeMapIndices[i], surfaces[i].data());
	}

	return texture;
}

TexturePtr CTextureLoader::CreateCubeTextureFromStreamPvr(Framework::CStream& stream)
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
	
	auto texture = CGraphicDevice::GetInstance().CreateCubeTexture(TEXTURE_FORMAT_PVRTC4, header->width);
		
	const uint8* surfaceDataPtr = pvrFile + sizeof(PVR_HEADER);
	for(unsigned int i = 0; i < 6; i++)
	{
		texture->UpdateCubeFace(c_cubeMapIndices[i], surfaceDataPtr);
		surfaceDataPtr += header->dataSize;
	}
	
	return texture;
}
