#pragma once

#include "palleon/ButtonBase.h"
#include "palleon/graphics/Sprite.h"
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

		SpritePtr				GetBackgroundSprite() const;

		void					SetReleasedTexture(const TexturePtr&);
		void					SetPressedTexture(const TexturePtr&);

		virtual void			SetSize(const CVector2&) override;

	protected:
		void					UpdateButtonState() override;

		SpritePtr				m_background;

		TexturePtr				m_releasedTexture;
		TexturePtr				m_pressedTexture;
	};
}
