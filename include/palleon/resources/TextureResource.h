#pragma once

#include "Resource.h"
#include "../graphics/Texture.h"

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
