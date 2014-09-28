#pragma once

#include <d3d11.h>
#include "Types.h"
#include "../Texture.h"
#include "Stream.h"

namespace Palleon
{
	class CDx11Texture : public CTexture
	{
	public:
									CDx11Texture(ID3D11Device*, ID3D11DeviceContext*, ID3D11Texture2D*);
		virtual						~CDx11Texture();

		static TexturePtr			Create(ID3D11Device*, ID3D11DeviceContext*, TEXTURE_FORMAT, uint32, uint32, uint32);
		static TexturePtr			CreateCube(ID3D11Device*, ID3D11DeviceContext*, TEXTURE_FORMAT, uint32);

		virtual void*				GetHandle() const override;

		virtual void				Update(uint32, const void*) override;
		virtual void				UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;

	protected:
		typedef std::pair<uint32, uint32> PitchPair;

		static TexturePtr			CreateFromStream(ID3D11Device*, ID3D11DeviceContext*, Framework::CStream&);
		static TexturePtr			CreateCubeFromStream(ID3D11Device*, ID3D11DeviceContext*, Framework::CStream&);

		void						UpdateSurface(unsigned int, unsigned int, unsigned int, const void*);
		static PitchPair			GetTexturePitches(TEXTURE_FORMAT, unsigned int, unsigned int);

		ID3D11Device*				m_device;
		ID3D11DeviceContext*		m_deviceContext;
		ID3D11Texture2D*			m_texture;
		ID3D11ShaderResourceView*	m_textureView;
	};
}
