#pragma once

#include <vector>
#include <functional>
#include <memory>
#include "Vector3.h"
#include "Quaternion.h"
#include "AnimationController.h"
#include "Matrix4.h"

namespace Palleon
{
	enum SCENE_NODE_TYPE
	{
		SCENE_NODE_PLAIN,
		SCENE_NODE_WIDGET
	};

	class CSceneNode;
	typedef std::shared_ptr<CSceneNode> SceneNodePtr;

	typedef CAnimationController<CSceneNode> SceneNodeAnimationController;
	typedef SceneNodeAnimationController::AnimationPtr SceneNodeAnimationPtr;

	class CSceneNode
	{
	public:
		typedef std::vector<SceneNodePtr> SceneNodeArray;
		typedef std::function<bool (const SceneNodePtr&)> TraversalFunction;

										CSceneNode();
		virtual							~CSceneNode();

		static SceneNodePtr				Create();

		virtual SCENE_NODE_TYPE			GetNodeType() const;

		void							AppendChild(const SceneNodePtr&);
		void							PrependChild(const SceneNodePtr&);
		void							AppendChildAfter(const SceneNodePtr&, const SceneNodePtr&);
		void							RemoveChild(const SceneNodePtr&);
		void							RemoveAllChildren();

		CSceneNode*						GetParent() const;

		std::string						GetName() const;
		void							SetName(const std::string&);

		SceneNodeAnimationController&	GetAnimationController();

		CVector3						GetPosition() const;
		void							SetPosition(const CVector3&);
		void							SetPositionHotspot(const CVector3&);
		
		CQuaternion						GetRotation() const;
		void							SetRotation(const CQuaternion&);

		CVector3						GetScale() const;
		void							SetScale(const CVector3&);

		bool							GetVisible() const;
		void							SetVisible(bool);

		CVector3						GetHotspot() const;
		void							SetHotspot(const CVector3&);

		virtual void					Update(float dt);

		void							TraverseNodes(const TraversalFunction&);

		void							UpdateTransformations();

		CMatrix4						GetWorldTransformation() const;
		bool							GetWorldVisibility() const;

	protected:
		CSceneNode*						m_parent;

		std::string						m_name;
		CVector3						m_position;
		CQuaternion						m_rotation;
		CVector3						m_scale;
		bool							m_visible;

		CVector3						m_hotspot;

		CMatrix4						m_worldTransformation;
		bool							m_worldVisibility;

		SceneNodeAnimationController	m_animationController;

		SceneNodeArray					m_children;
	};
}
