#pragma once

#include <vector>
#include "palleon/EmbedManager.h"

namespace Palleon
{
	class CWin32EmbedManager : public CEmbedManager
	{
	public:
		typedef std::vector<CEmbedRemoteCall> PendingEventArray;

									CWin32EmbedManager();
		virtual						~CWin32EmbedManager();

		static void					CreateInstance();
		static void					DestroyInstance();

		void						NotifyClient(const CEmbedRemoteCall&) override;

		const PendingEventArray&	GetPendingEvents() const;
		void						ClearPendingEvents();

	private:
		PendingEventArray			m_pendingEvents;
	};
}
