#include <memory>
#include <assert.h>
#include <cstring>
#include "palleon/resources/Package.h"
#include "palleon/resources/ResourceManager.h"
#include "palleon/resources/FontDescriptor.h"
#include "palleon/resources/EmitterDescriptor.h"
#include "palleon/resources/SceneDescriptor.h"
#include "palleon/resources/NinePatchDescriptor.h"
#include "xml/Node.h"
#include "xml/Parser.h"
#include "xml/Utils.h"

using namespace Palleon;

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
	return std::make_shared<CPackage>(name);
}

void CPackage::LoadDefinition()
{
	std::string packageRelPath = m_name + std::string("/package.xml");

	auto inputStream =  CResourceManager::GetInstance().MakeResourceStream(packageRelPath.c_str());
	std::unique_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(*inputStream));

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
			CResourceManager::GetInstance().LoadResource<CTextureResource>(item.name, itemPath);
			break;
		case ITEM_FONTDESCRIPTOR:
			CResourceManager::GetInstance().LoadResource<CFontDescriptor>(item.name, itemPath);
			break;
		case ITEM_NINEPATCHDESCRIPTOR:
			CResourceManager::GetInstance().LoadResource<CNinePatchDescriptor>(item.name, itemPath);
			break;
		case ITEM_EMITTERDESCRIPTOR:
			CResourceManager::GetInstance().LoadResource<CEmitterDescriptor>(item.name, itemPath);
			break;
		case ITEM_SCENEDESCRIPTOR:
			CResourceManager::GetInstance().LoadResource<CSceneDescriptor>(item.name, itemPath);
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
