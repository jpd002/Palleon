#ifndef _SCENENODE_H_
#define _SCENENODE_H_

#include "Vector2.h"
#include <vector>
#include <functional>
#include <memory>

namespace Athena
{
	enum SCENE_NODE_TYPE
	{
		SCENE_NODE_PLAIN,
		SCENE_NODE_WIDGET
	};

	class CSceneNode;
	typedef std::tr1::shared_ptr<CSceneNode> SceneNodePtr;

	class CSceneNode
	{
	public:
		typedef std::vector<SceneNodePtr> SceneNodeArray;
		typedef std::tr1::function<bool (CSceneNode*)> TraversalFunction;
	    
		virtual                     ~CSceneNode();

		static SceneNodePtr         Create();
	    
		virtual SCENE_NODE_TYPE     GetNodeType() const;

		void                        AppendChild(const SceneNodePtr&);
		void						AppendChildAfter(const SceneNodePtr&, const SceneNodePtr&);
		void                        RemoveChild(const SceneNodePtr&);

		CVector2                    GetPosition() const;
		void                        SetPosition(const CVector2&);
		
		CVector2                    GetScale() const;
		void                        SetScale(const CVector2&);
	    
		bool                        GetVisible() const;
		void                        SetVisible(bool);
	    
		virtual void                Update(float dt);
	    
		void                        TraverseNodes(const TraversalFunction&);
	    
		void                        UpdateTransformations();
	    
		CVector2                    GetWorldPosition() const;
		CVector2                    GetWorldScale() const;
		bool						GetWorldVisibility() const;
	    
	protected:    
									CSceneNode();
	    
		CSceneNode*                 m_parent;

		CVector2                    m_position;
		CVector2                    m_scale;
		float                       m_rotation;
		bool                        m_visible;
	    
		CVector2                    m_worldPosition;
		CVector2                    m_worldScale;
		bool                        m_worldVisibility;
	    
		SceneNodeArray              m_children;
	};
}

#endif
