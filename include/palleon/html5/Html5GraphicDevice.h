#pragma once

#include <EGL/EGL.h>
#include "../gles/GlEsGraphicDevice.h"

namespace Palleon
{
	class CHtml5GraphicDevice : public CGlEsGraphicDevice
	{
	public:
		static void						CreateInstance();
		
	private:
										CHtml5GraphicDevice();
		virtual							~CHtml5GraphicDevice();
	};
}
