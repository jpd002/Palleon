#include <cassert>
#include "palleon/Log.h"

using namespace Palleon;

CLog* CLog::m_instance = nullptr;

CLog& CLog::GetInstance()
{
	assert(m_instance != nullptr);
	return (*m_instance);
}
