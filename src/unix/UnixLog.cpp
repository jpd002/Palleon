#include <cassert>
#include <cstdarg>
#include <cstdio>
#include "palleon/unix/UnixLog.h"

using namespace Palleon;

void CUnixLog::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CUnixLog();
}

void CUnixLog::Print(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);
}
