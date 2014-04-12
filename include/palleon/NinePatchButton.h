#ifndef _ATHENA_NINEPATCHBUTTON_H_
#define _ATHENA_NINEPATCHBUTTON_H_

#include "ButtonBase.h"
#include "NinePatch.h"
#include <boost/signals2.hpp>

namespace Athena
{
	class CNinePatchButton;
	typedef std::shared_ptr<CNinePatchButton> NinePatchButtonPtr;

	class CNinePatchButton : public CButtonBase
	{
	public:
									CNinePatchButton();
		virtual						~CNinePatchButton();

		static NinePatchButtonPtr	Create();

		void						SetReleasedDescriptor(const CNinePatchDescriptor*);
		void						SetPressedDescriptor(const CNinePatchDescriptor*);

		virtual void				SetSize(const CVector2&);

	protected:
		void						UpdateButtonState();

		NinePatchPtr				m_background;

		const CNinePatchDescriptor*	m_releasedDescriptor;
		const CNinePatchDescriptor*	m_pressedDescriptor;
	};
}

#endif
