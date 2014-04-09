#pragma once

#include <vector>
#include "athena/gles/GlEsGraphicDevice.h"

namespace Athena
{
	class CIosGraphicDevice : public CGlEsGraphicDevice
	{
	public:
		static void							CreateInstance(bool, const CVector2&);
		static void							DestroyInstance();

	protected:				
		typedef std::vector<CMesh*> RenderQueue;

											CIosGraphicDevice(bool, const CVector2&);
		virtual								~CIosGraphicDevice();
	};
}
