#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "SceneNode.h"
#include "ILayoutable.h"

namespace Athena
{
	class CWidget : public CSceneNode, public ILayoutable
	{
	public:
									CWidget();
		virtual						~CWidget();

		virtual SCENE_NODE_TYPE		GetNodeType() const;

		void						SetPosition(const CVector3&);

		CVector2					GetSize() const;
		virtual void				SetSize(const CVector2&);

		virtual void				OnTouchPressed(bool);
		virtual void				OnTouchReleased(bool);
		virtual void				OnTouchMoved(const CVector2&);

	protected:
		CVector2					m_size;
	};

	typedef std::shared_ptr<CWidget> WidgetPtr;
}

#endif
