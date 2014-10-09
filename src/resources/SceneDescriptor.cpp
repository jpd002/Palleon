#include "palleon/resources/SceneDescriptor.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include <assert.h>
#include <algorithm>

using namespace Palleon;

CSceneDescriptor::CSceneDescriptor()
{

}

CSceneDescriptor::~CSceneDescriptor()
{

}

const CSceneDescriptor::NODE_INFO& CSceneDescriptor::GetRootNode() const
{
	return m_rootNode;
}

const CSceneDescriptor::NODE_INFO& CSceneDescriptor::GetLayoutRootNode() const
{
	return m_layoutRootNode;
}

const CSceneDescriptor::StyleMap& CSceneDescriptor::GetStyles() const
{
	return m_styles;
}

const CSceneDescriptor::MaterialMap& CSceneDescriptor::GetMaterials() const
{
	return m_materials;
}

const CSceneDescriptor::AnimationMap& CSceneDescriptor::GetAnimations() const
{
	return m_animations;
}

void CSceneDescriptor::Load(Framework::CStream& inputStream)
{
	std::unique_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(inputStream));

	LoadStyles(document.get());
	LoadMaterials(document.get());
	LoadAnimations(document.get());

	auto rootNode = document->Select("Scene/Root");
	m_rootNode.children = LoadNode(rootNode);

	auto layoutRootNode = document->Select("Scene/Layout");
	if(layoutRootNode)
	{
		m_layoutRootNode.children = LoadNode(layoutRootNode);
	}
}

void CSceneDescriptor::LoadStyles(Framework::Xml::CNode* document)
{
	auto styleNodes = document->SelectNodes("Scene/Styles/Style");
	for(const auto& node : styleNodes)
	{
		auto name = node->GetAttribute("Name");
		assert(name != nullptr);
		if(name == nullptr) continue;

		auto styleInfo = LoadItemInfo(node);

		assert(m_styles.find(name) == m_styles.end());
		m_styles.insert(std::make_pair(name, styleInfo));
	}
}

void CSceneDescriptor::LoadMaterials(Framework::Xml::CNode* document)
{
	auto materialNodes = document->SelectNodes("Scene/Materials/Material");
	for(const auto& node : materialNodes)
	{
		auto name = node->GetAttribute("Name");
		assert(name != nullptr);
		if(name == nullptr) continue;

		auto materialInfo = LoadItemInfo(node);

		assert(m_materials.find(name) == m_materials.end());
		m_materials.insert(std::make_pair(name, materialInfo));
	}
}

void CSceneDescriptor::LoadAnimations(Framework::Xml::CNode* document)
{
	auto animationNodes = document->SelectNodes("Scene/Animations/Animation");
	for(const auto& node : animationNodes)
	{
		auto name = node->GetAttribute("Name");
		assert(name != nullptr);
		if(name == nullptr) continue;

		ANIMATION_INFO animationInfo;
		animationInfo.properties = LoadItemInfo(node);

		auto animationKeyNodes = node->SelectNodes("Key");
		for(const auto& animationKeyNode : animationKeyNodes)
		{
			animationInfo.keys.push_back(LoadItemInfo(animationKeyNode));
		}

		assert(m_animations.find(name) == m_animations.end());
		m_animations.insert(std::make_pair(name, animationInfo));
	}
}

CSceneDescriptor::NodeInfoArray CSceneDescriptor::LoadNode(Framework::Xml::CNode* parentNode)
{
	NodeInfoArray result;
	for(const auto& node : parentNode->GetChildren())
	{
		if(!node->IsTag()) continue;

		auto name = node->GetAttribute("Name");

		NODE_INFO nodeInfo;
		nodeInfo.name		= name ? name : "";
		nodeInfo.type		= node->GetText();
		nodeInfo.children	= LoadNode(node);
		nodeInfo.properties	= LoadItemInfo(node);

		assert(name == nullptr || std::find_if(std::begin(result), std::end(result), [&] (const NODE_INFO& nodeInfo) { return nodeInfo.name == name; }) == std::end(result));
		result.push_back(nodeInfo);
	}
	return result;
}

CSceneDescriptor::ItemInfo CSceneDescriptor::LoadItemInfo(Framework::Xml::CNode* node)
{
	ItemInfo result;
	for(const auto& attribute : node->GetAttributes())
	{
		if(attribute.first == "Name") continue;
		result.insert(attribute);
	}
	return result;
}
