#include <Windows.h>
#include "EmbedWindow.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CoInitialize(NULL);

	{
		CEmbedWindow window;
		window.Center();
		window.Show(SW_SHOW);
		Framework::Win32::CWindow::StdMsgLoop(window);
	}

	CoUninitialize();

	return 0;
}
