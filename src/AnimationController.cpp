#include "athena/AnimationController.h"
#include <assert.h>

using namespace Athena;

CAnimationController::CAnimationController()
{

}

CAnimationController::~CAnimationController()
{

}

void CAnimationController::AddAnimation(const char* name, const AnimationPtr& animation)
{
	assert(animation);
	assert(m_animations.find(name) == std::end(m_animations));

	ANIMATION_STATE animationState;
	animationState.time = 0;
	animationState.playMode		= ANIMATION_PLAY_ONCE;
	animationState.animation	= animation;
	animationState.playing		= false;

	m_animations.insert(AnimationMap::value_type(name, animationState));
}

void CAnimationController::PlayAnimation(const char* name, ANIMATION_PLAY_MODE playMode, ANIMATION_PLAY_DIRECTION playDirection)
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

void CAnimationController::SetAnimationPlayDirection(const char* name, ANIMATION_PLAY_DIRECTION playDirection)
{
	auto animationIterator = m_animations.find(name);
	assert(animationIterator != std::end(m_animations));
	if(animationIterator == std::end(m_animations)) return;
	auto& animationState = animationIterator->second;
	animationState.playDirection	= playDirection;
}

bool CAnimationController::IsAnimationOver() const
{
	for(auto animationIterator = std::begin(m_animations);
		animationIterator != std::end(m_animations); animationIterator++)
	{
		const auto& animationState = animationIterator->second;
		if(animationState.playing) return false;
	}
	return true;
}

bool CAnimationController::IsAnimationOver(const char* name) const
{
	auto animationIterator = m_animations.find(name);
	assert(animationIterator != std::end(m_animations));
	if(animationIterator == std::end(m_animations)) return true;
	const auto& animationState = animationIterator->second;
	return !animationState.playing;
}

void CAnimationController::Update(CSceneNode* sceneNode, float dt)
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
					AnimationOver(sceneNode, animationIterator->first.c_str());
				}
			}
			break;
		}

		animation->Animate(sceneNode, animationState.time);
		
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
