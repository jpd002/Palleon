#include <assert.h>
#include <vector>
#include "palleon/gles/GlEsTexture.h"
#include "palleon/resources/TextureConverters.h"

using namespace Palleon;

uint32 GetNextPowerOfTwo(uint32 number)
{
	uint32 currentNumber = 1;
	while(currentNumber < number)
	{
		currentNumber <<= 1;
	}
	return currentNumber;
}

CGlEsTexture::CGlEsTexture(GLuint texture)
: m_texture(texture)
{

}

CGlEsTexture::~CGlEsTexture()
{
	glDeleteTextures(1, &m_texture);
}

TexturePtr CGlEsTexture::Create(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	GLuint texture = 0;
	glGenTextures(1, &texture);
	CHECKGLERROR();

	//TODO: Move sampling stuff out of here
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	auto result = std::make_shared<CGlEsTexture>(texture);
	result->m_width = width;
	result->m_height = height;
	result->m_format = textureFormat;
	result->m_isCube = false;
	return result;
}

TexturePtr CGlEsTexture::CreateCube(TEXTURE_FORMAT textureFormat, uint32 size)
{
	GLuint texture = 0;
	glGenTextures(1, &texture);
	CHECKGLERROR();

	//TODO: Move sampling stuff out of here
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	auto result = std::make_shared<CGlEsTexture>(texture);
	result->m_width = size;
	result->m_height = size;
	result->m_format = textureFormat;
	result->m_isCube = true;
	return result;
}

void CGlEsTexture::Update(uint32 mipLevel, const void* data)
{
	assert(m_isCube == false);
	UpdateTarget(GL_TEXTURE_2D, GL_TEXTURE_2D, data);
}

void CGlEsTexture::UpdateCubeFace(Palleon::TEXTURE_CUBE_FACE face, const void* data)
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

	UpdateTarget(GL_TEXTURE_CUBE_MAP, c_cubeFaces[face], data);
}

void* CGlEsTexture::GetHandle() const
{
	return reinterpret_cast<void*>(m_texture);
}

void CGlEsTexture::UpdateTarget(GLenum bindTarget, GLenum texImageTarget, const void* data)
{
	GLenum internalFormat = GL_RGB;
	GLenum format = GL_RGB;
	bool useCompressed = false;
	GLsizei imageSize = 0;
	std::vector<uint8> decodedTexture;
	
	switch(m_format)
	{
		case TEXTURE_FORMAT_RGB888:
			internalFormat = GL_RGB;
			format = GL_RGB;
			break;
		case TEXTURE_FORMAT_RGBA8888:
			internalFormat = GL_RGBA;
			format = GL_RGBA;
			break;
		case TEXTURE_FORMAT_BGRA8888:
#ifdef GL_BGRA		//Only supported on iOS
			internalFormat = GL_RGBA;
			format = GL_BGRA;
			break;
#else
			internalFormat = GL_RGBA;
			format = GL_RGBA;
			decodedTexture = CTextureConverters::BgraToRgba(m_width, m_height, data);
			data = decodedTexture.data();
			break;
#endif
#ifdef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
		case TEXTURE_FORMAT_PVRTC4:
			internalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			useCompressed = true;
			imageSize = (m_width * m_height) / 2;
			break;
#endif
		case TEXTURE_FORMAT_DXT1:
			internalFormat = GL_RGB;
			format = GL_RGB;
			decodedTexture = CTextureConverters::Dxt1ToRgb(m_width, m_height, data);
			data = decodedTexture.data();
			break;
		default:
			assert(0);
			break;
	}
	
	glBindTexture(bindTarget, m_texture);
	if(useCompressed)
	{
		glCompressedTexImage2D(texImageTarget, 0, internalFormat, m_width, m_height, 0, imageSize, data);
	}
	else
	{
		glTexImage2D(texImageTarget, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
	}
	CHECKGLERROR();
}
