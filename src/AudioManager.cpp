#include "palleon/AudioManager.h"
#include <assert.h>

using namespace Palleon;

CAudioManager* CAudioManager::m_instance = nullptr;

CAudioManager::~CAudioManager()
{

}

CAudioManager& CAudioManager::GetInstance()
{
	assert(m_instance != nullptr);
	return (*m_instance);
}
