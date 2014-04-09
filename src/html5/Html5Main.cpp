#include <iostream>
#include <emscripten.h>
#include "athena/Application.h"
#include "athena/html5/Html5GraphicDevice.h"

using namespace Athena;

void MainLoopProc()
{
	//std::cout << "Ticking" << std::endl;
}

int main(int argc, char** argv)
{
	std::cout << "Starting Meleon HTML5..." << std::endl;
	CHtml5GraphicDevice::CreateInstance();
	auto application = CreateApplication();
	std::cout << "Setting main loop..." << std::endl;
	emscripten_set_main_loop(&MainLoopProc, 0, true);
	return 0;
}
