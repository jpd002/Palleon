#include "palleon/LayoutNode.h"

using namespace Palleon;

CLayoutNode::CLayoutNode(unsigned int prefWidth, unsigned int prefHeight, unsigned int horzStretch, unsigned int vertStretch, const Palleon::LayoutablePtr& layoutable) 
: CLayoutObject(horzStretch, vertStretch)
, m_prefWidth(prefWidth)
, m_prefHeight(prefHeight)
, m_layoutable(layoutable)
{

}

CLayoutNode::~CLayoutNode()
{

}

unsigned int CLayoutNode::GetPreferredWidth()
{
	return m_prefWidth;
}

unsigned int CLayoutNode::GetPreferredHeight()
{
	return m_prefHeight;
}

void CLayoutNode::RefreshGeometry()
{
	unsigned int width = GetRight() - GetLeft();
	unsigned int height = GetBottom() - GetTop();

	m_layoutable->SetPosition(CVector3(GetLeft(), GetTop(), 0));
	m_layoutable->SetSize(CVector2(width, height));
}
