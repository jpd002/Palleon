#pragma once

namespace Palleon
{
	struct OffsetKeeper
	{
		uint32 Allocate(uint32 size)
		{
			uint32 result = currentOffset;
			currentOffset += size;
			return result;
		}

		uint32 currentOffset = 0;
	};
}
