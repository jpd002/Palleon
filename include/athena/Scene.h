#pragma once

#include "resources/SceneDescriptor.h"
#include "Material.h"
#include "SceneNode.h"

namespace Athena
{
	class CScene;
	typedef std::shared_ptr<CScene> ScenePtr;

	class CScene : public CSceneNode
	{
	public:
								CScene(const CSceneDescriptor*);
		virtual					~CScene();

		static ScenePtr			Create(const CSceneDescriptor*);
		
		SceneNodePtr			FindNode(const std::string&);

		template <typename ItemType>
		std::shared_ptr<ItemType> FindNode(const std::string& name)
		{
			auto result = FindNode(name);
			if(result)
			{
				return std::dynamic_pointer_cast<ItemType>(result);
			}
			else
			{
				return std::shared_ptr<ItemType>();
			}
		}

		SceneNodeAnimationPtr	GetAnimation(const std::string&) const;

	private:
		typedef std::map<std::string, MaterialPtr> MaterialArray;
		typedef std::map<std::string, SceneNodeAnimationPtr> AnimationArray;

		void				CreateScene(const CSceneDescriptor*);
		SceneNodePtr		CreateNode(const CSceneDescriptor::NODE_INFO&);
		void				CreateNodes(CSceneNode*, const CSceneDescriptor::NodeInfoArray&);
		void				CreateMaterials(const CSceneDescriptor*);
		void				CreateAnimations(const CSceneDescriptor*);

		MaterialPtr			GetMaterialFromItemInfo(const CSceneDescriptor::ItemInfo&) const;
		void				RegisterNodeAnimations(const SceneNodePtr&, const std::string&);

		MaterialArray		m_materials;
		AnimationArray		m_animations;
	};
}
