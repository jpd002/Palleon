#pragma once

#include "EmbedRemoteCall.h"

namespace Palleon
{
	class CEmbedManager
	{
	public:
		virtual						~CEmbedManager();

		static CEmbedManager&		GetInstance();

		virtual void				NotifyClient(const CEmbedRemoteCall&) = 0;

	protected:
		static CEmbedManager*		m_instance;
	};
}
