#include "palleon/Widget.h"

using namespace Palleon;

CWidget::CWidget()
: m_size(1, 1)
{

}

CWidget::~CWidget()
{

}

SCENE_NODE_TYPE CWidget::GetNodeType() const
{
	return SCENE_NODE_WIDGET;
}

void CWidget::SetPosition(const CVector3& position)
{
	CSceneNode::SetPosition(position);
}

CVector2 CWidget::GetSize() const
{
	return m_size;
}

void CWidget::SetSize(const CVector2& size)
{
	m_size = size;
}

void CWidget::OnTouchPressed(bool inside)
{

}

void CWidget::OnTouchReleased(bool inside)
{

}

void CWidget::OnTouchMoved(const CVector2& position)
{

}
