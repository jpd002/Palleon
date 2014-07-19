#include <algorithm>
#include <assert.h>
#include "palleon/SceneNode.h"

using namespace Palleon;

CSceneNode::CSceneNode()
: m_position(0, 0, 0)
, m_scale(1, 1, 1)
, m_visible(true)
, m_parent(nullptr)
, m_hotspot(0, 0, 0)
, m_worldTransformation(CMatrix4::MakeIdentity())
, m_worldVisibility(true)
{
	m_children.reserve(5);
}

CSceneNode::~CSceneNode()
{
	RemoveAllChildren();
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
	assert(node->m_parent == nullptr);
	assert(node.get() != this);
	assert(std::find(std::begin(m_children), std::end(m_children), node) == m_children.end());
	m_children.push_back(node);
	node->m_parent = this;
}

void CSceneNode::PrependChild(const SceneNodePtr& node)
{
	assert(node->m_parent == nullptr);
	assert(std::find(std::begin(m_children), std::end(m_children), node) == m_children.end());
	m_children.insert(m_children.begin(), node);
	node->m_parent = this;
}

void CSceneNode::AppendChildAfter(const SceneNodePtr& reference, const SceneNodePtr& child)
{
	assert(reference->m_parent == this);
	assert(child->m_parent == nullptr);
	assert(child.get() != this);

	auto nodeIterator(std::find(std::begin(m_children), std::end(m_children), reference));
	assert(nodeIterator != m_children.end());

	if(nodeIterator == m_children.end()) return;
	std::advance(nodeIterator, 1);
	m_children.insert(nodeIterator, child);
	child->m_parent = this;
}

void CSceneNode::RemoveChild(const SceneNodePtr& node)
{
	auto nodeIterator(std::find(std::begin(m_children), std::end(m_children), node));
	assert(nodeIterator != m_children.end());
	assert(node->m_parent == this);
	m_children.erase(nodeIterator);
	node->m_parent = nullptr;
}

void CSceneNode::RemoveAllChildren()
{
	for(auto& node : m_children)
	{
		assert(node->m_parent == this);
		node->m_parent = nullptr;
	}
	m_children.clear();
}

CSceneNode* CSceneNode::GetParent() const
{
	return m_parent;
}

const CSceneNode::SceneNodeArray& CSceneNode::GetChildren() const
{
	return m_children;
}

std::string CSceneNode::GetName() const
{
	return m_name;
}

void CSceneNode::SetName(const std::string& name)
{
	m_name = name;
}

SceneNodeAnimationController& CSceneNode::GetAnimationController()
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

void CSceneNode::SetPositionHotspot(const CVector3& position)
{
	m_position = position - m_hotspot;
}

CQuaternion CSceneNode::GetRotation() const
{
	return m_rotation;
}

void CSceneNode::SetRotation(const CQuaternion& rotation)
{
	m_rotation = rotation;
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

CVector3 CSceneNode::GetHotspot() const
{
	return m_hotspot;
}

void CSceneNode::SetHotspot(const CVector3& hotspot)
{
	m_hotspot = hotspot;
}

void CSceneNode::Update(float dt)
{
	m_animationController.Update(this, dt);
	for(const auto& child : m_children)
	{
		child->Update(dt);
	}
}

void CSceneNode::TraverseNodes(const TraversalFunction& traversalFunc)
{
	for(const auto& child : m_children)
	{
		if(traversalFunc(child))
		{
			child->TraverseNodes(traversalFunc);
		}
	}
}

void CSceneNode::UpdateTransformations()
{
	CMatrix4 applyHotspotTransformation(CMatrix4::MakeTranslation(-m_hotspot.x, -m_hotspot.y, -m_hotspot.z));
	CMatrix4 removeHotspotTransformation(CMatrix4::MakeTranslation(m_hotspot.x, m_hotspot.y, m_hotspot.z));
	CMatrix4 localRotation(m_rotation.ToMatrix());
	CMatrix4 localScale(CMatrix4::MakeScale(m_scale.x, m_scale.y, m_scale.z));
	CMatrix4 localPosition(CMatrix4::MakeTranslation(m_position.x, m_position.y, m_position.z));
	CMatrix4 localTransformation(applyHotspotTransformation * localScale * localRotation * localPosition * removeHotspotTransformation);

	if(m_parent == NULL)
	{
		m_worldTransformation = localTransformation;
	}
	else
	{
		m_worldTransformation = localTransformation * m_parent->m_worldTransformation;
		m_worldVisibility = m_visible && m_parent->m_worldVisibility;
	}

	for(auto nodeIterator(std::begin(m_children)); nodeIterator != std::end(m_children); nodeIterator++)
	{
		(*nodeIterator)->UpdateTransformations();
	}
}

CMatrix4 CSceneNode::GetWorldTransformation() const
{
	return m_worldTransformation;
}

bool CSceneNode::GetWorldVisibility() const
{
	return m_worldVisibility;
}
