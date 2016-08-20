#pragma once

namespace Palleon
{
	class CVulkanUberEffectGenerator
	{
	public:
		struct EFFECTCAPS
		{
			unsigned int hasVertexColor : 1;
			unsigned int reserved       : 31;
		};
		static_assert(sizeof(EFFECTCAPS) == 4, "Size of EFFECTCAPS must be 4 bytes.");
	};
}
