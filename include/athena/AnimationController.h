#pragma once

#include "Types.h"
#include "IAnimation.h"
#include <unordered_map>
#include <string>
#include <boost/signals2.hpp>

namespace Athena
{
	enum ANIMATION_PLAY_MODE
	{
		ANIMATION_PLAY_ONCE,
		ANIMATION_PLAY_LOOP
	};

	enum ANIMATION_PLAY_DIRECTION
	{
		ANIMATION_PLAY_FORWARD,
		ANIMATION_PLAY_BACKWARD
	};

	template <typename TargetType>
	class CAnimationController
	{
	public:
		typedef std::shared_ptr<IAnimation<TargetType>> AnimationPtr;
		typedef boost::signals2::signal<void (TargetType*, const char*)> AnimationOverEvent;

		CAnimationController()
		{

		}

		virtual ~CAnimationController()
		{

		}

		void AddAnimation(const char* name, const AnimationPtr& animation)
		{
			assert(animation);
			assert(m_animations.find(name) == std::end(m_animations));

			ANIMATION_STATE animationState;
			animationState.time = 0;
			animationState.playMode		= ANIMATION_PLAY_ONCE;
			animationState.animation	= animation;
			animationState.playing		= false;

			m_animations.insert(typename AnimationMap::value_type(name, animationState));
		}

		void PlayAnimation(const char* name, ANIMATION_PLAY_MODE playMode = ANIMATION_PLAY_ONCE, ANIMATION_PLAY_DIRECTION playDirection = ANIMATION_PLAY_FORWARD)
		{
			auto animationIterator = m_animations.find(name);
			assert(animationIterator != std::end(m_animations));
			if(animationIterator == std::end(m_animations)) return;
			auto& animationState = animationIterator->second;
			animationState.playMode			= playMode;
			animationState.playDirection	= playDirection;
			if(playDirection == ANIMATION_PLAY_FORWARD)
			{
				animationState.time = 0;
			}
			else
			{
				animationState.time = animationState.animation->GetLength();
			}
			animationState.playing			= true;
		}

		void SetAnimationPlayDirection(const char* name, ANIMATION_PLAY_DIRECTION playDirection)
		{
			auto animationIterator = m_animations.find(name);
			assert(animationIterator != std::end(m_animations));
			if(animationIterator == std::end(m_animations)) return;
			auto& animationState = animationIterator->second;
			animationState.playDirection	= playDirection;
		}

		bool IsAnimationOver() const
		{
			for(auto animationIterator = std::begin(m_animations);
				animationIterator != std::end(m_animations); animationIterator++)
			{
				const auto& animationState = animationIterator->second;
				if(animationState.playing) return false;
			}
			return true;
		}

		bool IsAnimationOver(const char* name) const
		{
			auto animationIterator = m_animations.find(name);
			assert(animationIterator != std::end(m_animations));
			if(animationIterator == std::end(m_animations)) return true;
			const auto& animationState = animationIterator->second;
			return !animationState.playing;
		}

		void Update(TargetType* target, float dt)
		{
			for(auto animationIterator = std::begin(m_animations);
				animationIterator != std::end(m_animations); animationIterator++)
			{
				auto& animationState = animationIterator->second;
				if(!animationState.playing) continue;
				const auto& animation = animationState.animation;

				//Check animation end
				switch(animationState.playMode)
				{
				case ANIMATION_PLAY_ONCE:
					{
						bool done = false;
						if(animationState.playDirection == ANIMATION_PLAY_FORWARD)
						{
							done = (animationState.time == animation->GetLength());
						}
						else
						{
							done = (animationState.time == 0);
						}
						if(done)
						{
							animationState.playing = false;
							AnimationOver(target, animationIterator->first.c_str());
						}
					}
					break;
				}

				animation->Animate(target, animationState.time);

				//Update time
				switch(animationState.playMode)
				{
				case ANIMATION_PLAY_ONCE:
					if(animationState.playDirection == ANIMATION_PLAY_FORWARD)
					{
						animationState.time = std::min<float>(animationState.time + dt, animation->GetLength());
					}
					else
					{
						animationState.time = std::max<float>(animationState.time - dt, 0);
					}
					break;
				case ANIMATION_PLAY_LOOP:
					if(animationState.playDirection == ANIMATION_PLAY_FORWARD)
					{
						if(animationState.time >= animation->GetLength())
						{
							animationState.time = 0;
						}
						else
						{
							animationState.time += dt;
						}
					}
					else
					{
						if(animationState.time <= 0)
						{
							animationState.time = animation->GetLength();
						}
						else
						{
							animationState.time -= dt;
						}
					}
					break;
				}
			}
		}

		AnimationOverEvent			AnimationOver;

	private:
		struct ANIMATION_STATE
		{
			AnimationPtr				animation;
			float						time;
			ANIMATION_PLAY_MODE			playMode;
			ANIMATION_PLAY_DIRECTION	playDirection;
			bool						playing;
		};

		typedef std::unordered_map<std::string, ANIMATION_STATE> AnimationMap;

		AnimationMap				m_animations;
	};
}
