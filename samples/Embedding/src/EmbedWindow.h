#pragma once

#include "win32/Window.h"
#include "Win32EmbedControl.h"

class CEmbedWindow : public Framework::Win32::CWindow
{
public:
									CEmbedWindow();
	virtual							~CEmbedWindow();

private:
	Palleon::CWin32EmbedControl*	m_embedControl = nullptr;
}
;