#pragma once

#include <memory>

namespace Athena
{
	class CAudioClip
	{
	public:
		virtual				~CAudioClip();
		
		virtual void		Play(bool loop) = 0;

	private:

	};

	typedef std::shared_ptr<CAudioClip> AudioClipPtr;
}
