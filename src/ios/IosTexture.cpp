#include "IosTexture.h"
#include <assert.h>
#include <vector>
#include "TgaImage.h"
#include "PvrImage.h"
#import <UIKit/UIKit.h>

using namespace Athena;

uint32 GetNextPowerOfTwo(uint32 number)
{
	uint32 currentNumber = 1;
	while(currentNumber < number)
	{
		currentNumber <<= 1;
	}
	return currentNumber;
}

CIosTexture::CIosTexture(GLuint texture, bool isCubeMap)
: m_texture(texture)
, m_isCubeMap(isCubeMap)
{

}

CIosTexture::~CIosTexture()
{
	glDeleteTextures(1, &m_texture);
}

TexturePtr CIosTexture::Create(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	GLuint texture = 0;
	glGenTextures(1, &texture);
	CHECKGLERROR();

	auto result = std::make_shared<CIosTexture>(texture);
	result->m_width = width;
	result->m_height = height;
	result->m_format = textureFormat;
	return result;
}

TexturePtr CIosTexture::CreateFromFile(const char* path)
{
	NSString* pathString = [NSString stringWithUTF8String: path];
	NSData* texData = [[NSData alloc] initWithContentsOfFile: pathString];
	assert(texData != NULL);
	GLuint texture = LoadFromData(texData);
	[texData release];	
	if(texture == 0)
	{
		return TexturePtr();
	}
	return TexturePtr(new CIosTexture(texture));
}

TexturePtr CIosTexture::CreateFromMemory(const void* data, uint32 size)
{
	NSData* texData = [[NSData alloc] initWithBytesNoCopy: const_cast<void*>(data) length: size freeWhenDone: NO];
	GLuint texture = LoadFromData(texData);
	[texData release];
	if(texture == 0)
	{
		return TexturePtr();
	}
	return TexturePtr(new CIosTexture(texture));	
}

void CIosTexture::Update(const void* data)
{
	GLenum internalFormat = GL_RGB;

	switch(m_format)
	{
		case TEXTURE_FORMAT_RGB888:
			internalFormat = GL_RGB;
			break;
		case TEXTURE_FORMAT_RGBA8888:
			internalFormat = GL_RGBA;
			break;
		default:
			assert(0);
			break;
	}
		
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, internalFormat, GL_UNSIGNED_BYTE, data);
	CHECKGLERROR();
}

TexturePtr CIosTexture::CreateCubeFromFile(const char* path)
{
	NSString* pathString = [NSString stringWithUTF8String: path];
	NSData* texData = [[NSData alloc] initWithContentsOfFile: pathString];
	assert(texData != NULL);
	GLuint texture = LoadCubeFromPVR(texData);
	[texData release];	
	if(texture == 0)
	{
		return TexturePtr();
	}
	return TexturePtr(new CIosTexture(texture, true));
}

void* CIosTexture::GetHandle() const
{
	return reinterpret_cast<void*>(m_texture);
}

bool CIosTexture::IsCubeMap() const
{
	return m_isCubeMap;
}

GLuint CIosTexture::LoadFromData(void* texDataPtr)
{
	NSData* texData = reinterpret_cast<NSData*>(texDataPtr);
	
	UIImage* image = [[UIImage alloc] initWithData: texData];
	if(image == nil)
	{
		return TryLoadTGA(texDataPtr);
	}
	
	GLuint srcWidth = CGImageGetWidth(image.CGImage);
	GLuint srcHeight = CGImageGetHeight(image.CGImage);
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

	GLuint dstWidth = GetNextPowerOfTwo(srcWidth);
	GLuint dstHeight = GetNextPowerOfTwo(srcHeight);

	void* imageData = malloc(dstHeight * dstWidth * 4);
		
	CGContextRef context = CGBitmapContextCreate(imageData, dstWidth, dstHeight, 8, 4 * dstWidth, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
	CGColorSpaceRelease(colorSpace);
	CGContextClearRect(context, CGRectMake(0, 0, dstWidth, dstHeight));
	CGContextTranslateCTM(context, 0, dstHeight - dstHeight);
	CGContextDrawImage(context, CGRectMake(0, 0, dstWidth, dstHeight), image.CGImage);
	
	for(unsigned int i = 0; i < dstWidth * dstHeight; i++)
	{
		uint32 srcPixel = reinterpret_cast<uint32*>(imageData)[i];
		uint16 a = static_cast<uint8>(srcPixel >>  0);
		uint16 b = static_cast<uint8>(srcPixel >>  8);
		uint16 g = static_cast<uint8>(srcPixel >> 16);
		uint16 r = static_cast<uint8>(srcPixel >> 24);
		uint16 dstPixel = (r >> 4) | ((g >> 4) << 4) | ((b >> 4) << 8) | ((a >> 4) << 12);
		reinterpret_cast<uint16*>(imageData)[i] = dstPixel;
	}
	
	GLuint result = 0;
	glGenTextures(1, &result);
	CHECKGLERROR();

	glBindTexture(GL_TEXTURE_2D, result);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dstWidth, dstHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, imageData);
	CHECKGLERROR();
	
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECKGLERROR();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	CGContextRelease(context);
	
	free(imageData);
	[image release];
	
	return result;
}

GLuint CIosTexture::TryLoadTGA(void* texDataPtr)
{
	NSData* texData = reinterpret_cast<NSData*>(texDataPtr);
	const uint8* tgaFile = reinterpret_cast<const uint8*>([texData bytes]);
	uint32 tgaFileSize = [texData length];
	
	if(tgaFileSize < sizeof(TGA_HEADER))
	{
		assert(0);
		return 0;
	}
	
	const TGA_HEADER* header = reinterpret_cast<const TGA_HEADER*>(tgaFile);
	if(header->imageType != 0x02)
	{
		assert(0);
		return 0;
	}
	
	if(header->bits != 32)
	{
		assert(0);
		return 0;
	}

	const uint32* srcImage = reinterpret_cast<const uint32*>(tgaFile + sizeof(TGA_HEADER));
	uint16* dstImage = new uint16[header->height * header->width];
	for(unsigned int y = 0; y < header->height; y++)
	{
		unsigned int srcY = header->height - y - 1;
		for(unsigned int x = 0; x < header->width; x++)
		{
			uint32 srcPixel = srcImage[x + (srcY * header->width)];
			uint16 b = static_cast<uint8>(srcPixel >>  0);
			uint16 g = static_cast<uint8>(srcPixel >>  8);
			uint16 r = static_cast<uint8>(srcPixel >> 16);
			uint16 a = static_cast<uint8>(srcPixel >> 24);
			uint16 dstPixel = (a >> 4) | ((b >> 4) << 4) | ((g >> 4) << 8) | ((r >> 4) << 12);
			dstImage[x + (y * header->width)] = dstPixel;
		}
	}
	
	GLuint result = 0;
	glGenTextures(1, &result);
	CHECKGLERROR();
	
	glBindTexture(GL_TEXTURE_2D, result);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header->width, header->height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, dstImage);
	CHECKGLERROR();
	
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECKGLERROR();
	
	glBindTexture(GL_TEXTURE_2D, 0);

	delete dstImage;
	
	return result;
}

GLuint CIosTexture::LoadCubeFromPVR(void* texDataPtr)
{
	NSData* texData = reinterpret_cast<NSData*>(texDataPtr);
	const uint8* pvrFile = reinterpret_cast<const uint8*>([texData bytes]);
	uint32 pvrFileSize = [texData length];
		
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
	
	GLuint result = 0;
	glGenTextures(1, &result);
	glBindTexture(GL_TEXTURE_CUBE_MAP, result);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECKGLERROR();
	
	static const GLenum c_cubeMapIndices[6] =
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};
	
	const uint8* surfaceDataPtr = pvrFile + sizeof(PVR_HEADER);
	for(unsigned int i = 0; i < 6; i++)
	{
		glCompressedTexImage2D(c_cubeMapIndices[i], 0, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, header->width, header->height, 0, header->dataSize, surfaceDataPtr);
		CHECKGLERROR();
		surfaceDataPtr += header->dataSize;
	}
	
	return result;
}
