#include "palleon/unix/UnixApplicationWindow.h"

using namespace Palleon;

int main(int argc, const char** argv)
{
	CUnixApplicationWindow applicationWindow;
	applicationWindow.Loop();
	return 0;
}
