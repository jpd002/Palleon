#pragma once

#include <string>

namespace Palleon
{
	class CMetalUberEffectGenerator
	{
	public:
		struct EFFECTCAPS
		{
			unsigned int	hasNormal		: 1;
			unsigned int	hasTexCoord0	: 1;
			unsigned int	hasTexture		: 1;
			unsigned int	hasShadowMap	: 1;
			unsigned int	reserved		: 28;
		};
		static_assert(sizeof(EFFECTCAPS) == 4, "EFFECTCAPS must be 4 bytes.");
		
		static std::string	GenerateLibrarySource(const EFFECTCAPS&);
		
	private:
	
	};
}
