#pragma once

#include <d3d11.h>
#include "Types.h"
#include "palleon/graphics/Texture.h"
#include "palleon/win32/Dx11ContextManager.h"

namespace Palleon
{
	class CDx11Texture : public CTexture
	{
	public:
									CDx11Texture(ID3D11Device*, CDx11ContextManager&, ID3D11Texture2D*);
		virtual						~CDx11Texture();

		static TexturePtr			Create(ID3D11Device*, CDx11ContextManager&, TEXTURE_FORMAT, uint32, uint32, uint32);
		static TexturePtr			CreateCube(ID3D11Device*, CDx11ContextManager&, TEXTURE_FORMAT, uint32);

		virtual void*				GetHandle() const override;

		virtual void				Update(uint32, const void*) override;
		virtual void				UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;

	protected:
		typedef std::pair<uint32, uint32> PitchPair;

		void						UpdateSurface(unsigned int, unsigned int, unsigned int, const void*);
		static PitchPair			GetTexturePitches(TEXTURE_FORMAT, unsigned int, unsigned int);

		ID3D11Device*				m_device;
		CDx11ContextManager&		m_contextManager;
		ID3D11Texture2D*			m_texture;
		ID3D11ShaderResourceView*	m_textureView;
	};
}
