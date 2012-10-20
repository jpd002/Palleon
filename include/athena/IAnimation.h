#pragma once

#include <memory>

namespace Athena
{
	class CSceneNode;
	typedef std::shared_ptr<CSceneNode> SceneNodePtr;

	class IAnimation
	{
	public:
		virtual				~IAnimation() {}
		virtual void		Animate(CSceneNode* target, float t) const = 0;
		virtual float		GetLength() const = 0;
	};

	typedef std::shared_ptr<IAnimation> AnimationPtr;
}
