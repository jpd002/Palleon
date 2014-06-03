#pragma once

#include "win32/Window.h"
#include "palleon/win32/Win32EmbedControl.h"

class CEmbedWindow : public Framework::Win32::CWindow
{
public:
									CEmbedWindow();
	virtual							~CEmbedWindow();

protected:
	long							OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	void							UpdateLayout();

	Palleon::CWin32EmbedControl*	m_embedControl = nullptr;
}
;