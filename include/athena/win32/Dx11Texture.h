#pragma once

#include <d3d11.h>
#include "Types.h"
#include "../Texture.h"
#include "Stream.h"

namespace Athena
{
	class CDx11Texture : public CTexture
	{
	public:
									CDx11Texture(ID3D11Device*, ID3D11DeviceContext*, ID3D11Texture2D*);
		virtual						~CDx11Texture();

		static TexturePtr			Create(ID3D11Device*, ID3D11DeviceContext*, TEXTURE_FORMAT, uint32, uint32);
		static TexturePtr			CreateFromFile(ID3D11Device*, ID3D11DeviceContext*, const char*);
		static TexturePtr			CreateFromMemory(ID3D11Device*, ID3D11DeviceContext*, const void*, uint32);
	
		static TexturePtr			CreateCubeFromFile(ID3D11Device*, const char*);

		virtual void*				GetHandle() const;
		ID3D11ShaderResourceView*	GetTextureView() const;

		void						Update(const void*);

	protected:
		static TexturePtr			CreateFromStream(ID3D11Device*, ID3D11DeviceContext*, Framework::CStream&);

		ID3D11Device*				m_device;
		ID3D11DeviceContext*		m_deviceContext;
		ID3D11Texture2D*			m_texture;
		ID3D11ShaderResourceView*	m_textureView;
	};
}
