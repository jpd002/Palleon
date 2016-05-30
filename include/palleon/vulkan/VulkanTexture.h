#pragma once

#include "palleon/graphics/Texture.h"

namespace Palleon
{
	class CVulkanTexture : public CTexture
	{
	public:
		void*    GetHandle() const override;
		
		void    Update(uint32, const void*) override;
		void    UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;
		
	private:
	
	};
}
