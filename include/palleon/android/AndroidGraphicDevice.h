#pragma once

#include "../gles/GlEsGraphicDevice.h"

namespace Palleon
{
	class CAndroidGraphicDevice : public CGlEsGraphicDevice
	{
	public:
		static void						CreateInstance(int, int, float);
		
		virtual SharedGraphicContextPtr	CreateSharedContext() override;
		
	private:
										CAndroidGraphicDevice(int, int, float);
		virtual							~CAndroidGraphicDevice();
	};
}
