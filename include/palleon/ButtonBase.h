#pragma once

#include "Widget.h"
#include "Label.h"
#include <boost/signals2.hpp>

namespace Palleon
{
	class CButtonBase;
	typedef std::shared_ptr<CButtonBase> ButtonBasePtr;

	class CButtonBase : public CWidget
	{
	public:
		typedef boost::signals2::signal<void (CButtonBase*)> PressEventType;

							CButtonBase();
		virtual				~CButtonBase();

		void				SetText(const std::string&);
		void				SetFont(const CFontDescriptor*);

		virtual void		SetSize(const CVector2&);

		virtual void		OnTouchPressed(bool) override;
		virtual void		OnTouchReleased(bool) override;

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
