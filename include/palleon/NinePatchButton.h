#pragma once

#include "palleon/ButtonBase.h"
#include "palleon/graphics/NinePatch.h"
#include <boost/signals2.hpp>

namespace Palleon
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

		const CNinePatchDescriptor*	m_releasedDescriptor = nullptr;
		const CNinePatchDescriptor*	m_pressedDescriptor = nullptr;
	};
}
