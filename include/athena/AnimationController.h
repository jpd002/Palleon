#pragma once

#include "Types.h"
#include "IAnimation.h"
#include <unordered_map>
#include <string>
#include <boost/signals2.hpp>

namespace Athena
{
	class CSceneNode;

	enum ANIMATION_PLAY_MODE
	{
		ANIMATION_PLAY_ONCE,
		ANIMATION_PLAY_LOOP
	};

	class CAnimationController
	{
	public:
		typedef boost::signals2::signal<void (CSceneNode*, const char*)> AnimationOverEvent;

									CAnimationController();
		virtual						~CAnimationController();

		void						AddAnimation(const char*, const AnimationPtr&);
		void						PlayAnimation(const char*, ANIMATION_PLAY_MODE = ANIMATION_PLAY_ONCE);

		bool						IsAnimationOver() const;
		bool						IsAnimationOver(const char*) const;

		void						Update(CSceneNode*, float);

		AnimationOverEvent			AnimationOver;

	private:
		struct ANIMATION_STATE
		{
			AnimationPtr			animation;
			float					time;
			ANIMATION_PLAY_MODE		playMode;
			bool					playing;
		};

		typedef std::unordered_map<std::string, ANIMATION_STATE> AnimationMap;

		AnimationMap				m_animations;
	};
}
