#include <cassert>
#include <android/log.h>
#include "palleon/android/AndroidLog.h"

using namespace Palleon;

#define LOG_NAME "Palleon"

void CAndroidLog::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CAndroidLog();
}

void CAndroidLog::Print(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	__android_log_vprint(ANDROID_LOG_INFO, LOG_NAME, fmt, ap);
	va_end(ap);
}
