#include <assert.h>
#include <vector>
#include "athena/ios/IosTexture.h"

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

CIosTexture::CIosTexture(GLuint texture)
: m_texture(texture)
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

	//TODO: Move sampling stuff out of here
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	auto result = std::make_shared<CIosTexture>(texture);
	result->m_width = width;
	result->m_height = height;
	result->m_format = textureFormat;
	result->m_isCube = false;
	return result;
}

TexturePtr CIosTexture::CreateCube(TEXTURE_FORMAT textureFormat, uint32 size)
{
	GLuint texture = 0;
	glGenTextures(1, &texture);
	CHECKGLERROR();

	//TODO: Move sampling stuff out of here
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	auto result = std::make_shared<CIosTexture>(texture);
	result->m_width = size;
	result->m_height = size;
	result->m_format = textureFormat;
	result->m_isCube = true;
	return result;
}

void DecodeDxt1Block(uint8* dstData, size_t dstPitch, const uint8* srcData)
{
	static const size_t pixelSize = 3;
	
	uint32 c0 = srcData[0] | (srcData[1] << 8);
	uint32 c1 = srcData[2] | (srcData[3] << 8);
	uint32 bits = srcData[4] | (srcData[5] << 8) | (srcData[6] << 16) | (srcData[7] << 24);
	
	uint32 b0 = (c0 & 0x1f) << 3;
	uint32 g0 = ((c0 >> 5) & 0x3f) << 2;
	uint32 r0 = ((c0 >> 11) & 0x1f) << 3;
	
	uint32 b1 = (c1 & 0x1f) << 3;
	uint32 g1 = ((c1 >> 5) & 0x3f) << 2;
	uint32 r1 = ((c1 >> 11) & 0x1f) << 3;
	
	for(unsigned int y = 0; y < 4; y++)
	{
		uint8* dstLinePtr = dstData + (y * dstPitch);
		for(unsigned int x = 0; x < 4; x++)
		{
			uint32 control = bits & 0x03;
			bits >>= 2;
			switch(control)
			{
				case 0:
					dstLinePtr[0] = r0;
					dstLinePtr[1] = g0;
					dstLinePtr[2] = b0;
					break;
				case 1:
					dstLinePtr[0] = r1;
					dstLinePtr[1] = g1;
					dstLinePtr[2] = b1;
					break;
				case 2:
					if(c0 > c1)
					{
						dstLinePtr[0] = (2 * r0 + r1) / 3;
						dstLinePtr[1] = (2 * g0 + g1) / 3;
						dstLinePtr[2] = (2 * b0 + b1) / 3;
					}
					else
					{
						dstLinePtr[0] = (r0 + r1) / 2;
						dstLinePtr[1] = (g0 + g1) / 2;
						dstLinePtr[2] = (b0 + b1) / 2;
					}
					break;
				case 3:
					if(c0 > c1)
					{
						dstLinePtr[0] = (r0 + 2 * r1) / 3;
						dstLinePtr[1] = (g0 + 2 * g1) / 3;
						dstLinePtr[2] = (b0 + 2 * b1) / 3;
					}
					else
					{
						dstLinePtr[0] = 0;
						dstLinePtr[1] = 0;
						dstLinePtr[2] = 0;
					}
					break;
			}
			dstLinePtr += pixelSize;
		}
	}
}

std::vector<uint8> Dxt1ToRgb(uint32 width, uint32 height, const void* data)
{
	static const size_t pixelSize = 3;
	std::vector<uint8> dstData(width * height * pixelSize);
	const uint8* srcData = reinterpret_cast<const uint8*>(data);
	const size_t dstPitch = width * pixelSize;
	for(unsigned int y = 0; y < height; y += 4)
	{
		uint8* dstLinePtr = dstData.data() + (dstPitch * y);
		for(unsigned int x = 0; x < width; x += 4)
		{
			DecodeDxt1Block(dstLinePtr, dstPitch, srcData);
			dstLinePtr += (pixelSize * 4);
			srcData += 8;
		}
	}
	return dstData;
}

void CIosTexture::Update(uint32 mipLevel, const void* data)
{
	assert(m_isCube == false);
	
	GLenum internalFormat = GL_RGB;

	switch(m_format)
	{
		case TEXTURE_FORMAT_RGB888:
			internalFormat = GL_RGB;
			break;
		case TEXTURE_FORMAT_RGBA8888:
			internalFormat = GL_RGBA;
			break;
		case TEXTURE_FORMAT_DXT1:
			{
				auto decodedTexture = Dxt1ToRgb(m_width, m_height, data);
				glBindTexture(GL_TEXTURE_2D, m_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, decodedTexture.data());
				return;
				break;
			}
		default:
			assert(0);
			break;
	}
	
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, internalFormat, GL_UNSIGNED_BYTE, data);
	CHECKGLERROR();
}

void CIosTexture::UpdateCubeFace(Athena::TEXTURE_CUBE_FACE face, const void* data)
{
	assert(m_isCube == true);
	
	static const GLenum c_cubeFaces[TEXTURE_CUBE_FACE_MAX] =
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};
	
	GLenum internalFormat = GL_RGB;
	bool useCompressed = false;
	GLsizei imageSize = 0;
	
	switch(m_format)
	{
		case TEXTURE_FORMAT_RGB888:
			internalFormat = GL_RGB;
			break;
		case TEXTURE_FORMAT_RGBA8888:
			internalFormat = GL_RGBA;
			break;
		case TEXTURE_FORMAT_PVRTC4:
			internalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			useCompressed = true;
			imageSize = (m_width * m_height) / 2;
			break;
		default:
			assert(0);
			break;
	}
		
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	if(useCompressed)
	{
		glCompressedTexImage2D(c_cubeFaces[face], 0, internalFormat, m_width, m_height, 0, imageSize, data);
	}
	else
	{
		glTexImage2D(c_cubeFaces[face], 0, internalFormat, m_width, m_height, 0, internalFormat, GL_UNSIGNED_BYTE, data);
	}
	CHECKGLERROR();
}

void* CIosTexture::GetHandle() const
{
	return reinterpret_cast<void*>(m_texture);
}
