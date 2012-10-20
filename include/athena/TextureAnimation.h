#pragma once

#include "IAnimation.h"
#include "Texture.h"
#include <vector>

namespace Athena
{
	class CTextureAnimation : public IAnimation
	{
	public:
							CTextureAnimation();
		virtual				~CTextureAnimation();

		void				AddFrame(const TexturePtr&, float);

		void				Animate(CSceneNode*, float) const;
		float				GetLength() const;

	private:
		struct FRAME
		{
			TexturePtr		texture;
			float			length;
		};
		typedef std::vector<FRAME> FrameArray;

		FrameArray			m_frames;
		float				m_length;
	};
};
