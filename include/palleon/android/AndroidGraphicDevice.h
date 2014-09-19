#pragma once

#include "../gles/GlEsGraphicDevice.h"

namespace Palleon
{
	class CAndroidGraphicDevice : public CGlEsGraphicDevice
	{
	public:
		static void						CreateInstance(int, int);
		
	private:
										CAndroidGraphicDevice(int, int);
		virtual							~CAndroidGraphicDevice();
	};
}
