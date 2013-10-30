#include "athena/resources/SceneDescriptor.h"
#include "StdStream.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include <assert.h>
#include <algorithm>

using namespace Athena;

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

const CSceneDescriptor::MaterialMap& CSceneDescriptor::GetMaterials() const
{
	return m_materials;
}

const CSceneDescriptor::AnimationMap& CSceneDescriptor::GetAnimations() const
{
	return m_animations;
}

void CSceneDescriptor::Load(const char* path)
{
	Framework::CStdStream inputStream(path, "rb");
	std::unique_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(inputStream));

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

	auto rootNode = document->Select("Scene/Root");
	m_rootNode.children = LoadNode(rootNode);
}

CSceneDescriptor::NodeInfoArray CSceneDescriptor::LoadNode(Framework::Xml::CNode* parentNode)
{
	NodeInfoArray result;
	for(const auto& node : parentNode->GetChildren())
	{
		if(!node->IsTag()) continue;

		auto name = node->GetAttribute("Name");
		assert(name != nullptr);
		if(name == nullptr) continue;

		NODE_INFO nodeInfo;
		nodeInfo.name		= name;
		nodeInfo.type		= node->GetText();
		nodeInfo.children	= LoadNode(node);
		nodeInfo.properties	= LoadItemInfo(node);

		assert(std::find_if(std::begin(result), std::end(result), [&] (const NODE_INFO& nodeInfo) { return nodeInfo.name == name; }) == std::end(result));
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
