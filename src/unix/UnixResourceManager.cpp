#include <cassert>
#include "StdStream.h"
#include "make_unique.h"
#include "palleon/unix/UnixResourceManager.h"

#define DATA_BASE "../data/"

using namespace Palleon;

void CUnixResourceManager::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CUnixResourceManager();
}

void CUnixResourceManager::DestroyInstance()
{
	assert(m_instance != nullptr);
	delete m_instance;
	m_instance = nullptr;
}

std::string CUnixResourceManager::MakeResourcePath(const std::string& name) const
{
	return std::string(DATA_BASE) + name;
}

CResourceManager::StreamPtr CUnixResourceManager::MakeResourceStream(const std::string& name) 
const
{
	auto resourcePath = MakeResourcePath(name);
	return std::make_unique<Framework::CStdStream>(resourcePath.c_str(), "rb");
}

