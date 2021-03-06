#pragma once

#include <string>

namespace Palleon
{
	enum KEY_CODE
	{
		KEY_CODE_NONE,
		KEY_CODE_W,
		KEY_CODE_A,
		KEY_CODE_S,
		KEY_CODE_D,
		KEY_CODE_ARROW_UP,
		KEY_CODE_ARROW_DOWN,
		KEY_CODE_ARROW_LEFT,
		KEY_CODE_ARROW_RIGHT
	};

	class CApplication
	{
	public:
		virtual				~CApplication() {};

		virtual void		Update(float) = 0;

		virtual void		NotifySizeChanged() {}

		virtual void		NotifyMouseMove(int, int) {}
		virtual void		NotifyMouseDown() {}
		virtual void		NotifyMouseUp() {}
		virtual void		NotifyMouseWheel(int) {}

		virtual void		NotifyKeyDown(KEY_CODE) {}
		virtual void		NotifyKeyUp(KEY_CODE) {}

		virtual void		NotifyInputCancelled() {}

		//Embedding notifications
		virtual std::string	NotifyExternalCommand(const std::string&) { return ""; }
	};
}

extern Palleon::CApplication* CreateApplication(bool isEmbedding = false);
