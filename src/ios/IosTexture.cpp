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
