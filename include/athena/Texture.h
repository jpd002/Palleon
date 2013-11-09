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
		TEXTURE_FORMAT_DXT1,
		TEXTURE_FORMAT_MAX
	};

	extern const uint32 c_textureFormatSize[TEXTURE_FORMAT_MAX];

	class CTexture
	{
	public:
							CTexture();
		virtual				~CTexture();

		virtual void*		GetHandle() const = 0;

	protected:
		TEXTURE_FORMAT		m_format;
		uint32				m_width;
		uint32				m_height;
	};

	typedef std::shared_ptr<CTexture> TexturePtr;
}
