#pragma once

#include <memory>

namespace Palleon
{
	class CAudioClip
	{
	public:
		virtual				~CAudioClip();
		
		virtual void		Play(bool loop) = 0;
		virtual void		Stop() = 0;

	private:

	};

	typedef std::shared_ptr<CAudioClip> AudioClipPtr;
}
