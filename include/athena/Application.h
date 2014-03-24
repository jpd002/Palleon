#pragma once

namespace Athena
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
		virtual			~CApplication() {};

		virtual void	Update(float) = 0;

		virtual void	NotifyMouseMove(int, int) {}
		virtual void	NotifyMouseDown() {}
		virtual void	NotifyMouseUp() {}

		virtual void	NotifyKeyDown(KEY_CODE) {}
		virtual void	NotifyKeyUp(KEY_CODE) {}

		virtual void	NotifyInputCancelled() {}
	};
}

extern Athena::CApplication* CreateApplication();
