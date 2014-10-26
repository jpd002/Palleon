#pragma once

#include "IAnimation.h"
#include "palleon/graphics/Texture.h"
#include <vector>

namespace Palleon
{
	class CSceneNode;

	class CTextureAnimation : public IAnimation<CSceneNode>
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
