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
	};

	typedef std::tr1::shared_ptr<CTexture> TexturePtr;
}

#endif
