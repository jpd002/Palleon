#include <memory>
#include <assert.h>
#include "athena/resources/Package.h"
#include "athena/resources/ResourceManager.h"
#include "athena/resources/FontDescriptor.h"
#include "athena/resources/EmitterDescriptor.h"
#include "athena/resources/SceneDescriptor.h"
#include "athena/resources/NinePatchDescriptor.h"
#include "xml/Node.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include "StdStream.h"

using namespace Athena;

CPackage::CPackage(const char* name)
: m_name(name)
{
	LoadDefinition();
	LoadItems();
}

CPackage::~CPackage()
{
	ReleaseItems();
}

PackagePtr CPackage::Create(const char* name)
{
	return PackagePtr(new CPackage(name));
}

void CPackage::LoadDefinition()
{
	std::string packageRelPath = m_name + std::string("/package.xml");
	std::string packagePath = CResourceManager::GetInstance().MakeResourcePath(packageRelPath.c_str());

	Framework::CStdStream inputStream(packagePath.c_str(), "rb");
	std::unique_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(inputStream));

	Framework::Xml::CNode* packageNode = document->Select("Package");
	assert(packageNode != NULL);

	for(const auto& itemNode : packageNode->GetChildren())
	{
		if(!itemNode->IsTag()) continue;
		const char* itemType = itemNode->GetText();
		const char* itemSource = itemNode->GetAttribute("Source");
		assert(itemSource != NULL);

		ITEM newItem;
		newItem.name = itemSource;

		if(!strcmp(itemType, "Texture"))
		{
			newItem.type = ITEM_TEXTURE;
		}
		else if(!strcmp(itemType, "FontDescriptor"))
		{
			newItem.type = ITEM_FONTDESCRIPTOR;
		}
		else if(!strcmp(itemType, "NinePatchDescriptor"))
		{
			newItem.type = ITEM_NINEPATCHDESCRIPTOR;
		}
		else if(!strcmp(itemType, "EmitterDescriptor"))
		{
			newItem.type = ITEM_EMITTERDESCRIPTOR;
		}
		else if(!strcmp(itemType, "Scene"))
		{
			newItem.type = ITEM_SCENEDESCRIPTOR;
		}
		else
		{
			assert(0);
			continue;
		}

		m_items.push_back(newItem);
	}
}

void CPackage::LoadItems()
{
	for(const auto& item : m_items)
	{
		auto itemPath = m_name + std::string("/") + item.name;
		switch(item.type)
		{
		case ITEM_TEXTURE:
			CResourceManager::GetInstance().LoadResource<CTextureResource>(item.name.c_str(), itemPath.c_str());
			break;
		case ITEM_FONTDESCRIPTOR:
			CResourceManager::GetInstance().LoadResource<CFontDescriptor>(item.name.c_str(), itemPath.c_str());
			break;
		case ITEM_NINEPATCHDESCRIPTOR:
			CResourceManager::GetInstance().LoadResource<CNinePatchDescriptor>(item.name.c_str(), itemPath.c_str());
			break;
		case ITEM_EMITTERDESCRIPTOR:
			CResourceManager::GetInstance().LoadResource<CEmitterDescriptor>(item.name.c_str(), itemPath.c_str());
			break;
		case ITEM_SCENEDESCRIPTOR:
			CResourceManager::GetInstance().LoadResource<CSceneDescriptor>(item.name.c_str(), itemPath.c_str());
			break;
		}
	}
}

void CPackage::ReleaseItems()
{
	for(const auto& item : m_items)
	{
		CResourceManager::GetInstance().ReleaseResource(item.name.c_str());
	}
}
