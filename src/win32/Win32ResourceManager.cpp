#include <assert.h>
#include <string>
#include "athena/win32/Win32ResourceManager.h"

#define DATA_BASE	"./data/"

using namespace Athena;

CWin32ResourceManager::CWin32ResourceManager()
{

}

CWin32ResourceManager::~CWin32ResourceManager()
{

}

void CWin32ResourceManager::CreateInstance()
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CWin32ResourceManager();
}

void CWin32ResourceManager::DestroyInstance()
{
	assert(m_instance != NULL);
	delete m_instance;
	m_instance = NULL;
}

std::string CWin32ResourceManager::MakeResourcePath(const char* name) const
{
	return std::string(DATA_BASE) + std::string(name);
}
