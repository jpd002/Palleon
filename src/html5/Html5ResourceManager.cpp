#include "palleon/html5/Html5ResourceManager.h"

using namespace Palleon;

CHtml5ResourceManager::CHtml5ResourceManager()
{

}

CHtml5ResourceManager::~CHtml5ResourceManager()
{

}

void CHtml5ResourceManager::CreateInstance()
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CHtml5ResourceManager();
}

void CHtml5ResourceManager::DestroyInstance()
{
	assert(m_instance != NULL);
	if(m_instance == NULL) return;
	delete m_instance;
}

std::string CHtml5ResourceManager::MakeResourcePath(const std::string& name) const
{
	return name;
}
