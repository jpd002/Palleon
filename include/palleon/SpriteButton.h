#pragma once

#include "ButtonBase.h"
#include "Sprite.h"
#include <boost/signals2.hpp>

namespace Palleon
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

		void					SetBackgroundAlphaBlendingMode(Palleon::ALPHA_BLENDING_MODE);

		virtual void			SetSize(const CVector2&) override;

	protected:
		void					UpdateButtonState();

		SpritePtr				m_background;

		TexturePtr				m_releasedTexture;
		TexturePtr				m_pressedTexture;
	};
}
