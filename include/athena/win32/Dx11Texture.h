#pragma once

#include <d3d11.h>
#include "Types.h"
#include "../Texture.h"

namespace Athena
{
	class CDx11Texture : public CTexture
	{
	public:
									CDx11Texture(ID3D11Texture2D*, ID3D11ShaderResourceView*);
		virtual						~CDx11Texture();

		static TexturePtr			Create(ID3D11Device*, TEXTURE_FORMAT, uint32, uint32);
		static TexturePtr			CreateFromFile(ID3D11Device*, const char*);
		static TexturePtr			CreateFromMemory(ID3D11Device*, const void*, uint32);
	
		static TexturePtr			CreateCubeFromFile(ID3D11Device*, const char*);

		virtual void*				GetHandle() const;
		ID3D11ShaderResourceView*	GetTextureView() const;

		void						Update(const void*);

	protected:
		ID3D11Texture2D*			m_texture;
		ID3D11ShaderResourceView*	m_textureView;
	};
}
