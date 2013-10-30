#pragma once

#include "Resource.h"
#include "../Texture.h"

namespace Athena
{
	class CTextureResource : public CResource
	{
	public:
							CTextureResource();
		virtual				~CTextureResource();

		virtual void		Load(const char*) override;

		TexturePtr			GetTexture() const;

	private:
		TexturePtr			m_texture;
	};
}
