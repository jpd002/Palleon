#ifndef _SCENENODE_H_
#define _SCENENODE_H_

#include <vector>
#include <functional>
#include <memory>
#include "Vector3.h"
#include "AnimationController.h"

namespace Athena
{
	enum SCENE_NODE_TYPE
	{
		SCENE_NODE_PLAIN,
		SCENE_NODE_WIDGET
	};

	class CSceneNode;
	typedef std::shared_ptr<CSceneNode> SceneNodePtr;

	class CSceneNode
	{
	public:
		typedef std::vector<SceneNodePtr> SceneNodeArray;
		typedef std::function<bool (CSceneNode*)> TraversalFunction;

									CSceneNode();
		virtual						~CSceneNode();

		static SceneNodePtr			Create();

		virtual SCENE_NODE_TYPE		GetNodeType() const;

		void						AppendChild(const SceneNodePtr&);
		void						PrependChild(const SceneNodePtr&);
		void						AppendChildAfter(const SceneNodePtr&, const SceneNodePtr&);
		void						RemoveChild(const SceneNodePtr&);

		CAnimationController&		GetAnimationController();

		CVector3					GetPosition() const;
		void						SetPosition(const CVector3&);
		
		CVector3					GetScale() const;
		void						SetScale(const CVector3&);

		bool						GetVisible() const;
		void						SetVisible(bool);

		virtual void				Update(float dt);

		void						TraverseNodes(const TraversalFunction&);

		void						UpdateTransformations();

		CVector3					GetWorldPosition() const;
		CVector3					GetWorldScale() const;
		bool						GetWorldVisibility() const;

	protected:
		CSceneNode*					m_parent;

		CVector3					m_position;
		CVector3					m_scale;
		bool						m_visible;

		CVector3					m_worldPosition;
		CVector3					m_worldScale;
		bool						m_worldVisibility;

		CAnimationController		m_animationController;

		SceneNodeArray				m_children;
	};
}

#endif
