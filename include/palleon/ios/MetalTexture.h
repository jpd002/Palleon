#pragma once

#include "palleon/Texture.h"

namespace Palleon
{
	class CMetalTexture : public CTexture
	{
	public:
									CMetalTexture();
		virtual						~CMetalTexture();
		
		void						Update(uint32, const void*) override;
		void						UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;
		
		void*						GetHandle() const override;
		
	private:
		
	};
}
