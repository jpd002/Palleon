#include "palleon/win32/Dx11SharedGraphicContext.h"
#include "palleon/win32/Dx11GraphicDevice.h"

using namespace Palleon;

CDx11SharedGraphicContext::CDx11SharedGraphicContext()
{
	static_cast<Palleon::CDx11GraphicDevice&>(Palleon::CGraphicDevice::GetInstance()).CreateDeviceContextForThread();
}

CDx11SharedGraphicContext::~CDx11SharedGraphicContext()
{
	static_cast<Palleon::CDx11GraphicDevice&>(Palleon::CGraphicDevice::GetInstance()).DestroyDeviceContextForThread();
}

void CDx11SharedGraphicContext::Flush()
{
	static_cast<Palleon::CDx11GraphicDevice&>(Palleon::CGraphicDevice::GetInstance()).FlushDeviceContextForThread();
}

