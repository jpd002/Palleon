#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "SceneNode.h"

namespace Athena
{
	class CWidget : public CSceneNode
	{
	public:
									CWidget();
		virtual                     ~CWidget();
	    
		virtual SCENE_NODE_TYPE     GetNodeType() const;
	    
		CVector2                    GetSize() const;
		virtual void                SetSize(const CVector2&);
	    
		virtual void                OnTouchPressed(bool);
		virtual void                OnTouchReleased(bool);
	    
	protected:
		CVector2                    m_size;
	};
}

#endif
