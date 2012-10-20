#include <algorithm>
#include <assert.h>
#include "athena/SceneNode.h"

using namespace Athena;

CSceneNode::CSceneNode()
: m_position(0, 0, 0)
, m_scale(1, 1, 1)
, m_visible(true)
, m_parent(NULL)
, m_worldPosition(0, 0, 0)
, m_worldScale(1, 1, 1)
, m_worldVisibility(true)
{
	m_children.reserve(5);
}

CSceneNode::~CSceneNode()
{

}

SceneNodePtr CSceneNode::Create()
{
	return SceneNodePtr(new CSceneNode());
}

SCENE_NODE_TYPE CSceneNode::GetNodeType() const
{
	return SCENE_NODE_PLAIN;
}

void CSceneNode::AppendChild(const SceneNodePtr& node)
{
	assert(node->m_parent == NULL);
	assert(std::find(std::begin(m_children), std::end(m_children), node) == m_children.end());
	m_children.push_back(node);
	node->m_parent = this;
}

void CSceneNode::PrependChild(const SceneNodePtr& node)
{
	assert(node->m_parent == NULL);
	assert(std::find(std::begin(m_children), std::end(m_children), node) == m_children.end());
	m_children.insert(m_children.begin(), node);
	node->m_parent = this;
}

void CSceneNode::AppendChildAfter(const SceneNodePtr& reference, const SceneNodePtr& child)
{
	assert(reference->m_parent == this);
	assert(child->m_parent == NULL);
	assert(child.get() != this);

	auto nodeIterator(std::find(std::begin(m_children), std::end(m_children), reference));
	assert(nodeIterator != m_children.end());

	if(nodeIterator == m_children.end()) return;
	m_children.insert(nodeIterator, child);
	child->m_parent = this;
}

void CSceneNode::RemoveChild(const SceneNodePtr& node)
{
	auto nodeIterator(std::find(std::begin(m_children), std::end(m_children), node));
	assert(nodeIterator != m_children.end());
	assert(node->m_parent == this);
	m_children.erase(nodeIterator);
	node->m_parent = NULL;
}

CAnimationController& CSceneNode::GetAnimationController()
{
	return m_animationController;
}

CVector3 CSceneNode::GetPosition() const
{
	return m_position;
}

void CSceneNode::SetPosition(const CVector3& position)
{
	m_position = position;
}

CVector3 CSceneNode::GetScale() const
{
	return m_scale;
}

void CSceneNode::SetScale(const CVector3& scale)
{
	m_scale = scale;
}

bool CSceneNode::GetVisible() const
{
	return m_visible;
}

void CSceneNode::SetVisible(bool visible)
{
	m_visible = visible;
}

void CSceneNode::Update(float dt)
{
	m_animationController.Update(this, dt);
	for(auto nodeIterator(std::begin(m_children)); nodeIterator != std::end(m_children); nodeIterator++)
	{
		(*nodeIterator)->Update(dt);
	}
}

void CSceneNode::TraverseNodes(const TraversalFunction& traversalFunc)
{
	if(!traversalFunc(this))
	{
		return;
	}

	for(auto nodeIterator(std::begin(m_children)); nodeIterator != std::end(m_children); nodeIterator++)
	{
		(*nodeIterator)->TraverseNodes(traversalFunc);
	}
}

void CSceneNode::UpdateTransformations()
{
	if(m_parent == NULL)
	{
		m_worldPosition		= m_position;
		m_worldScale		= m_scale;
		m_worldVisibility	= m_visible;
	}
	else
	{
		m_worldPosition	 = m_parent->m_worldPosition;
		m_worldPosition.x += m_position.x * m_parent->m_worldScale.x;
		m_worldPosition.y += m_position.y * m_parent->m_worldScale.y;
		m_worldPosition.z += m_position.z * m_parent->m_worldScale.z;

		m_worldScale.x = m_scale.x * m_parent->m_worldScale.x;
		m_worldScale.y = m_scale.y * m_parent->m_worldScale.y;
		m_worldScale.z = m_scale.z * m_parent->m_worldScale.z;

		m_worldVisibility = m_visible && m_parent->m_worldVisibility;
	}

	for(auto nodeIterator(std::begin(m_children)); nodeIterator != std::end(m_children); nodeIterator++)
	{
		(*nodeIterator)->UpdateTransformations();
	}
}

CVector3 CSceneNode::GetWorldPosition() const
{
	return m_worldPosition;
}

CVector3 CSceneNode::GetWorldScale() const
{
	return m_worldScale;
}

bool CSceneNode::GetWorldVisibility() const
{
	return m_worldVisibility;
}
