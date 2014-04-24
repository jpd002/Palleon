#pragma once

#include <vector>
#include "Types.h"

namespace Palleon
{
	class CTextureConverters
	{
	public:
		static std::vector<uint8>	Dxt1ToRgb(uint32 width, uint32 height, const void* data);
		static std::vector<uint8>	BgraToRgba(uint32 width, uint32 height, const void* data);
	};
}
