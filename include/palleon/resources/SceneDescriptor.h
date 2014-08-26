#pragma once

#include "Resource.h"
#include "xml/Node.h"
#include <map>
#include <string>
#include <vector>

namespace Palleon
{
	class CSceneDescriptor : public CResource
	{
	public:
		typedef std::map<std::string, std::string> ItemInfo;

		struct NODE_INFO;
		typedef std::vector<NODE_INFO> NodeInfoArray;

		struct NODE_INFO
		{
			std::string		name;
			std::string		type;
			ItemInfo		properties;
			NodeInfoArray	children;
		};

		typedef std::map<std::string, ItemInfo> StyleMap;

		typedef std::vector<ItemInfo> AnimationKeyInfo;

		struct ANIMATION_INFO
		{
			ItemInfo			properties;
			AnimationKeyInfo	keys;

		};
		typedef std::map<std::string, ANIMATION_INFO> AnimationMap;

		typedef std::map<std::string, ItemInfo> MaterialMap;

							CSceneDescriptor();
		virtual				~CSceneDescriptor();

		const NODE_INFO&	GetRootNode() const;
		const NODE_INFO&	GetLayoutRootNode() const;
		const StyleMap&		GetStyles() const;
		const MaterialMap&	GetMaterials() const;
		const AnimationMap&	GetAnimations() const;

		void				Load(const char*) override;

	private:
		NodeInfoArray		LoadNode(Framework::Xml::CNode*);
		ItemInfo			LoadItemInfo(Framework::Xml::CNode*);

		void				LoadStyles(Framework::Xml::CNode*);
		void				LoadMaterials(Framework::Xml::CNode*);
		void				LoadAnimations(Framework::Xml::CNode*);

		StyleMap			m_styles;
		MaterialMap			m_materials;
		AnimationMap		m_animations;
		NODE_INFO			m_rootNode;
		NODE_INFO			m_layoutRootNode;
	};
}
