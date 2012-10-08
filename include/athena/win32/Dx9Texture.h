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
									CDx9Texture(IDirect3DBaseTexture9*);
		virtual						~CDx9Texture();

		static TexturePtr			Create(IDirect3DDevice9*, TEXTURE_FORMAT, uint32, uint32);
		static TexturePtr			CreateFromFile(IDirect3DDevice9*, const char*);
		static TexturePtr			CreateFromMemory(IDirect3DDevice9*, const void*, uint32);
	
		static TexturePtr			CreateCubeFromFile(IDirect3DDevice9*, const char*);

		virtual void*				GetHandle() const;

		void						Update(const void*);

	protected:
		IDirect3DBaseTexture9*		m_texture;
	};
}

#endif
