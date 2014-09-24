#pragma once

#include "Resource.h"
#include "../Texture.h"

namespace Palleon
{
	class CTextureResource : public CResource
	{
	public:
							CTextureResource();
		virtual				~CTextureResource();

		virtual void		Load(Framework::CStream&) override;

		TexturePtr			GetTexture() const;

	private:
		TexturePtr			m_texture;
	};
}
