#ifndef _ANIMATEDSPRITE_H_
#define _ANIMATEDSPRITE_H_

#include "Sprite.h"
#include <boost/signals2.hpp>

namespace Athena
{
	class CAnimatedSprite;
	typedef std::shared_ptr<CAnimatedSprite> AnimatedSpritePtr;

	class CAnimatedSprite : public CSprite
	{
	public:
		typedef boost::signals2::signal<void ()> AnimationOverEventType;

									CAnimatedSprite();
		virtual						~CAnimatedSprite();

		static AnimatedSpritePtr	Create();

		void						Update(float);

		void						AddFrame(const TexturePtr&, float);
		void						PlayOnce();
		void						PlayLoop();

		AnimationOverEventType		AnimationOver;

	private:
		enum STATE
		{
			STATE_STOPPED,
			STATE_PLAYING_ONCE,
			STATE_PLAYING_LOOP,
		};

		struct FRAME
		{
			TexturePtr				texture;
			float					timeLength;
		};

		typedef std::vector<FRAME> FrameArray;

		void						UpdateTexture();

		FrameArray					m_frames;
		STATE						m_state;

		float						m_currentFrameTime;
		int							m_currentFrameIndex;
	};
}

#endif
