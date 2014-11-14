#include "palleon/EmbedManager.h"

using namespace Palleon;

CEmbedManager* CEmbedManager::m_instance = nullptr;

CEmbedManager::~CEmbedManager()
{

}

CEmbedManager& CEmbedManager::GetInstance()
{
	assert(m_instance != nullptr);
	return (*m_instance);
}
