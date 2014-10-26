#pragma once

#include "../gles/GlEsGraphicDevice.h"

namespace Palleon
{
	class CAndroidGraphicDevice : public CGlEsGraphicDevice
	{
	public:
		static void						CreateInstance(int, int, float);
		
	private:
										CAndroidGraphicDevice(int, int, float);
		virtual							~CAndroidGraphicDevice();
	};
}
