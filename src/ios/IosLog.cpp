#include <cassert>
#include <cstdarg>
#include "palleon/ios/IosLog.h"
#include <UIKit/UIKit.h>

using namespace Palleon;

void CIosLog::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CIosLog();
}

void CIosLog::Print(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	NSLogv([NSString stringWithUTF8String: fmt], args);
	va_end(args);
}
