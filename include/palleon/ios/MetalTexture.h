#pragma once

#include <Metal/Metal.h>
#include "palleon/Texture.h"

namespace Palleon
{
	class CMetalTexture : public CTexture
	{
	public:
									CMetalTexture(id<MTLTexture>);
		virtual						~CMetalTexture();
		
		static TexturePtr			Create(id<MTLDevice>, TEXTURE_FORMAT, uint32, uint32);
		static TexturePtr			CreateCube(id<MTLDevice>, TEXTURE_FORMAT, uint32);
		
		void						Update(uint32, const void*) override;
		void						UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;
		
		void*						GetHandle() const override;
		
	private:
		id<MTLTexture>				m_texture = nil;
	};
}
