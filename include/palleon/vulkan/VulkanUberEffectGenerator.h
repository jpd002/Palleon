#pragma once

#include "Stream.h"
#include "nuanceur/Builder.h"

namespace Palleon
{
	class CVulkanUberEffectGenerator
	{
	public:
		struct EFFECTCAPS
		{
			unsigned int hasVertexColor : 1;
			unsigned int hasTexture     : 1;
			unsigned int reserved       : 30;
		};
		static_assert(sizeof(EFFECTCAPS) == 4, "Size of EFFECTCAPS must be 4 bytes.");
		
		static void GenerateVertexShader(Framework::CStream&, const EFFECTCAPS&);
		static void GenerateFragmentShader(Framework::CStream&, const EFFECTCAPS&);
		
	private:
		static Nuanceur::CShaderBuilder BuildVertexShader(const EFFECTCAPS&);
		static Nuanceur::CShaderBuilder BuildFragmentShader(const EFFECTCAPS&);
	};
}
