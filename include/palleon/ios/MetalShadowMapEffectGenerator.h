#pragma once

#include <string>

namespace Palleon
{
	class CMetalShadowMapEffectGenerator
	{
	public:
		struct EFFECTCAPS
		{
			unsigned int	hasNormal		: 1;
			unsigned int	hasTexCoord0	: 1;
		};
		
		static std::string	GenerateLibrarySource(const EFFECTCAPS&);
		
	private:
	
	};
}
