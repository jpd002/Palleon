#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <memory>

namespace Athena
{
	enum TEXTURE_FORMAT
	{
		TEXTURE_FORMAT_UNKNOWN,
		TEXTURE_FORMAT_RGB888,
	};

	class CTexture
	{
	public:
		virtual				~CTexture();

		virtual void*		GetHandle() const = 0;
	};

	typedef std::shared_ptr<CTexture> TexturePtr;
}

#endif
