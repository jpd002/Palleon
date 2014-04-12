#pragma once

#include <memory>
#include "Types.h"

namespace Athena
{
	enum TEXTURE_FORMAT
	{
		TEXTURE_FORMAT_UNKNOWN,
		TEXTURE_FORMAT_RGB888,
		TEXTURE_FORMAT_RGBA8888,
		TEXTURE_FORMAT_BGRA8888,
		TEXTURE_FORMAT_DXT1,
		TEXTURE_FORMAT_DXT3,
		TEXTURE_FORMAT_DXT5,
		TEXTURE_FORMAT_PVRTC4,
		TEXTURE_FORMAT_MAX
	};

	enum TEXTURE_CUBE_FACE
	{
		TEXTURE_CUBE_FACE_POSITIVE_X,
		TEXTURE_CUBE_FACE_NEGATIVE_X,
		TEXTURE_CUBE_FACE_POSITIVE_Y,
		TEXTURE_CUBE_FACE_NEGATIVE_Y,
		TEXTURE_CUBE_FACE_POSITIVE_Z,
		TEXTURE_CUBE_FACE_NEGATIVE_Z,
		TEXTURE_CUBE_FACE_MAX
	};

	extern const uint32 c_textureFormatSize[TEXTURE_FORMAT_MAX];

	class CTexture
	{
	public:
							CTexture();
		virtual				~CTexture();

		bool				IsCube() const;

		virtual void		Update(uint32, const void*) = 0;
		virtual void		UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) = 0;
		virtual void*		GetHandle() const = 0;

	protected:
		TEXTURE_FORMAT		m_format;
		uint32				m_width;
		uint32				m_height;
		uint32				m_mipCount;
		bool				m_isCube;
	};

	typedef std::shared_ptr<CTexture> TexturePtr;
}
