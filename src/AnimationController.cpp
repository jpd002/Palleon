#include "athena/AnimationController.h"
#include <assert.h>


#include <Windows.h>
#include "athena/SceneNode.h"

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

void CAnimationController::PlayAnimation(const char* name, ANIMATION_PLAY_MODE playMode)
{
	auto animationIterator = m_animations.find(name);
	assert(animationIterator != std::end(m_animations));
	if(animationIterator == std::end(m_animations)) return;
	auto& animationState = animationIterator->second;
	animationState.playMode			= playMode;
	animationState.time				= 0;
	animationState.playing			= true;
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
			if(animationState.time == animation->GetLength())
			{
				animationState.playing = false;
				AnimationOver(animationIterator->first.c_str());
			}
			break;
		}

		animation->Animate(sceneNode, animationState.time);
		
		//Update time
		switch(animationState.playMode)
		{
		case ANIMATION_PLAY_ONCE:
			animationState.time = std::min<float>(animationState.time + dt, animation->GetLength());
			break;
		case ANIMATION_PLAY_LOOP:
			if(animationState.time >= animation->GetLength())
			{
				animationState.time = 0;
			}
			else
			{
				animationState.time += dt;
			}
			break;
		}
	}
}
