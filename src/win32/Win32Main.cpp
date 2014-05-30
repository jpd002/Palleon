#include <Windows.h>
#include "palleon/win32/Win32ApplicationWindow.h"
#include "palleon/win32/Win32EmbedServer.h"

using namespace Palleon;

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR cmdLine, int)
{
	CoInitialize(NULL);

	if(!strcmp(cmdLine, "embed"))
	{
		CWin32EmbedServer embedServer;
		embedServer.Loop();
	}
	else
	{
		CWin32ApplicationWindow applicationWindow;
		applicationWindow.Center(NULL);
		applicationWindow.Show(SW_SHOW);
		applicationWindow.Loop();
	}

	CoUninitialize();

	return 0;
}
