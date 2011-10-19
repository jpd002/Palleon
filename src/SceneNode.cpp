#include <algorithm>
#include <assert.h>
#include "athena/SceneNode.h"

using namespace Athena;

CSceneNode::CSceneNode()
: m_position(0, 0)
, m_scale(1, 1)
, m_rotation(0)
, m_visible(true)
, m_parent(NULL)
, m_worldPosition(0, 0)
, m_worldScale(1, 1)
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
    assert(std::find(m_children.begin(), m_children.end(), node) == m_children.end());
    m_children.push_back(node);
    node->m_parent = this;
}

void CSceneNode::AppendChildAfter(const SceneNodePtr& reference, const SceneNodePtr& child)
{
	assert(reference->m_parent == this);
	assert(child->m_parent == NULL);
	assert(child.get() != this);

	SceneNodeArray::iterator nodeIterator(std::find(m_children.begin(), m_children.end(), reference));
	assert(nodeIterator != m_children.end());

	if(nodeIterator == m_children.end()) return;
	m_children.insert(nodeIterator, child);
	child->m_parent = this;
}

void CSceneNode::RemoveChild(const SceneNodePtr& node)
{
    SceneNodeArray::iterator nodeIterator(std::find(m_children.begin(), m_children.end(), node));
    assert(nodeIterator != m_children.end());
    assert(node->m_parent == this);
    m_children.erase(nodeIterator);
    node->m_parent = NULL;
}

CVector2 CSceneNode::GetPosition() const
{
    return m_position;
}

void CSceneNode::SetPosition(const CVector2& position)
{
    m_position = position;
}

CVector2 CSceneNode::GetScale() const
{
    return m_scale;
}

void CSceneNode::SetScale(const CVector2& scale)
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
    for(SceneNodeArray::iterator nodeIterator(m_children.begin());
        nodeIterator != m_children.end(); nodeIterator++)
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
    
    for(SceneNodeArray::const_iterator nodeIterator(m_children.begin());
        nodeIterator != m_children.end(); nodeIterator++)
    {
        (*nodeIterator)->TraverseNodes(traversalFunc);
    }
}

void CSceneNode::UpdateTransformations()
{
    if(m_parent == NULL)
    {
        m_worldPosition     = m_position;
        m_worldScale        = m_scale;
        m_worldVisibility   = m_visible;
    }
    else
    {
        m_worldPosition    = m_parent->m_worldPosition;
        m_worldPosition.x += m_position.x * m_parent->m_worldScale.x;
        m_worldPosition.y += m_position.y * m_parent->m_worldScale.y;
        
        m_worldScale.x     = m_scale.x * m_parent->m_worldScale.x;
        m_worldScale.y     = m_scale.y * m_parent->m_worldScale.y;
        
        m_worldVisibility  = m_visible && m_parent->m_worldVisibility;
    }
    
    for(SceneNodeArray::const_iterator nodeIterator(m_children.begin());
        nodeIterator != m_children.end(); nodeIterator++)
    {
        (*nodeIterator)->UpdateTransformations();
    }    
}

CVector2 CSceneNode::GetWorldPosition() const
{
    return m_worldPosition;
}

CVector2 CSceneNode::GetWorldScale() const
{
    return m_worldScale;
}

bool CSceneNode::GetWorldVisibility() const
{
	return m_worldVisibility;
}
