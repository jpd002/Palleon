#ifndef _ATHENA_BUTTONBASE_H_
#define _ATHENA_BUTTONBASE_H_

#include "Widget.h"
#include "Label.h"
#include <boost/signals2.hpp>

namespace Athena
{
	class CButtonBase;
	typedef std::shared_ptr<CButtonBase> ButtonBasePtr;

	class CButtonBase : public CWidget
	{
	public:
		typedef boost::signals2::signal<void (CButtonBase*)> PressEventType;

							CButtonBase();
		virtual				~CButtonBase();

		void				SetText(const char*);
		void				SetFont(const CFontDescriptor*);

		virtual void		SetSize(const CVector2&);

		virtual void		OnTouchPressed(bool);
		virtual void		OnTouchReleased(bool);

		PressEventType		Press;

	protected:
		enum BUTTON_STATE
		{
			STATE_PRESSED,
			STATE_RELEASED,
		};

		virtual void		UpdateButtonState() = 0;

		LabelPtr			m_label;

		BUTTON_STATE		m_state;
	};
}

#endif
