#ifndef _DX9TEXTURE_H_
#define _DX9TEXTURE_H_

#include <d3d9.h>
#include "Types.h"
#include "../Texture.h"

namespace Athena
{
	class CDx9Texture : public CTexture
	{
	public:
								CDx9Texture(IDirect3DDevice9*, const char*);
								CDx9Texture(IDirect3DDevice9*, const void*, uint32);
								CDx9Texture(IDirect3DDevice9*, const void*, TEXTURE_FORMAT, uint32, uint32);
		virtual					~CDx9Texture();

		IDirect3DTexture9*		GetTexture() const;

	private:
		IDirect3DTexture9*		m_texture;
	};
}

#endif
