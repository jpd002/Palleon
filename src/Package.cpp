#include <boost/scoped_ptr.hpp>
#include "athena/Package.h"
#include "athena/ResourceManager.h"
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
    boost::scoped_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(&inputStream));

	Framework::Xml::CNode* packageNode = document->Select("Package");
	assert(packageNode != NULL);

	for(Framework::Xml::CNode::NodeIterator nodeIterator(packageNode->GetChildrenBegin());
		nodeIterator != packageNode->GetChildrenEnd(); nodeIterator++)
	{
		Framework::Xml::CNode* itemNode = (*nodeIterator);
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
	for(ItemArray::const_iterator itemIterator(m_items.begin());
		m_items.end() != itemIterator; itemIterator++)
	{
		const ITEM& item(*itemIterator);
		std::string itemPath = m_name + std::string("/") + item.name;
		switch(item.type)
		{
		case ITEM_TEXTURE:
			CResourceManager::GetInstance().LoadTexture(item.name.c_str(), itemPath.c_str());
			break;
		case ITEM_FONTDESCRIPTOR:
			CResourceManager::GetInstance().LoadFontDescriptor(item.name.c_str(), itemPath.c_str());
			break;
		}
	}
}

void CPackage::ReleaseItems()
{
	for(ItemArray::const_iterator itemIterator(m_items.begin());
		m_items.end() != itemIterator; itemIterator++)
	{
		const ITEM& item(*itemIterator);
		switch(item.type)
		{
		case ITEM_TEXTURE:
			CResourceManager::GetInstance().ReleaseTexture(item.name.c_str());
			break;
		case ITEM_FONTDESCRIPTOR:
			CResourceManager::GetInstance().ReleaseFontDescriptor(item.name.c_str());
			break;
		}
	}
}
