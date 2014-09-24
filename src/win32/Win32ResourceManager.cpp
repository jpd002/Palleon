#include <assert.h>
#include <string>
#include "StdStream.h"
#include "make_unique.h"
#include "palleon/win32/Win32ResourceManager.h"

#define DATA_BASE	"./data/"

using namespace Palleon;

CWin32ResourceManager::CWin32ResourceManager()
{

}

CWin32ResourceManager::~CWin32ResourceManager()
{

}

void CWin32ResourceManager::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CWin32ResourceManager();
}

void CWin32ResourceManager::DestroyInstance()
{
	assert(m_instance != nullptr);
	delete m_instance;
	m_instance = nullptr;
}

std::string CWin32ResourceManager::MakeResourcePath(const std::string& name) const
{
	return std::string(DATA_BASE) + name;
}

CResourceManager::StreamPtr CWin32ResourceManager::MakeResourceStream(const std::string& name) const
{
	auto resourcePath = MakeResourcePath(name);
	return std::make_unique<Framework::CStdStream>(resourcePath.c_str(), "rb");
}
