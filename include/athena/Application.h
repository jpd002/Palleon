#ifndef _GAMEPROGRAM_H_
#define _GAMEPROGRAM_H_

namespace Athena
{
	class CApplication
	{
	public:
		virtual			~CApplication() {};

		virtual void	Update(float) = 0;

		virtual void	NotifyMouseMove(unsigned int, unsigned int) {}
		virtual void	NotifyMouseDown() {}
		virtual void	NotifyMouseUp() {}
	};
}

extern Athena::CApplication* CreateApplication();

#endif
