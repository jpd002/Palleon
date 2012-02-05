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
		virtual						~CDx9Texture();

		static TexturePtr			CreateFromFile(IDirect3DDevice9*, const char*);
		static TexturePtr			CreateFromMemory(IDirect3DDevice9*, const void*, uint32);
		static TexturePtr			CreateFromRawData(IDirect3DDevice9*, const void*, TEXTURE_FORMAT, uint32, uint32);
		
		static TexturePtr			CreateCubeFromFile(IDirect3DDevice9*, const char*);

		IDirect3DBaseTexture9*		GetTexture() const;

	protected:
									CDx9Texture(IDirect3DBaseTexture9*);

		IDirect3DBaseTexture9*		m_texture;
	};
}

#endif
