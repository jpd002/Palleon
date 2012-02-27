#ifndef _ATHENA_SPRITEBUTTON_H_
#define _ATHENA_SPRITEBUTTON_H_

#include "ButtonBase.h"
#include "Sprite.h"
#include <boost/signals2.hpp>

namespace Athena
{
	class CSpriteButton;
	typedef std::shared_ptr<CSpriteButton> SpriteButtonPtr;

	class CSpriteButton : public CButtonBase
	{
	public:
								CSpriteButton();
		virtual					~CSpriteButton();

		static SpriteButtonPtr	Create();

		void					SetReleasedTexture(const TexturePtr&);
		void					SetPressedTexture(const TexturePtr&);

		virtual void			SetSize(const CVector2&);

	protected:
		void					UpdateButtonState();

		SpritePtr				m_background;

		TexturePtr				m_releasedTexture;
		TexturePtr				m_pressedTexture;
	};
}

#endif
