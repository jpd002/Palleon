#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Widget.h"
#include "Sprite.h"
#include "Label.h"
#include <boost/signals2.hpp>

namespace Athena
{
	class CButton;
	typedef std::tr1::shared_ptr<CButton> ButtonPtr;

	class CButton : public CWidget
	{
	public:
		typedef boost::signals2::signal<void ()> PressedEventType;
	    
		virtual             ~CButton();

		static ButtonPtr    Create();
	    
		void                SetText(const char*);
		void                SetFont(const CFontDescriptor*);
	    
		void                SetReleasedTexture(const TexturePtr&);
		void                SetPressedTexture(const TexturePtr&);
		void                SetTextureExtends(float, float);
	    
		virtual void        SetSize(const CVector2&);
	    
		virtual void        OnTouchPressed(bool);
		virtual void        OnTouchReleased(bool);
	    
		PressedEventType    OnPressed;
	    
	protected:
		enum BUTTON_STATE
		{
			STATE_PRESSED,
			STATE_RELEASED,
		};
	    
							CButton();
		void                UpdateButtonState();
	    
		SpritePtr           m_background;
		LabelPtr            m_label;
	    
		BUTTON_STATE        m_state;
	    
		TexturePtr			m_pressedTexture;
		TexturePtr			m_releasedTexture;
	};
}

#endif
