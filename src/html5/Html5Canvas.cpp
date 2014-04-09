#include <iostream>
#include <emscripten.h>
#include "athena/html5/Html5Canvas.h"
#include "athena/html5/Html5GraphicDevice.h"

using namespace Athena;

CHtml5Canvas::CHtml5Canvas()
{
	CHtml5GraphicDevice::CreateInstance();

	m_application = CreateApplication();
}

CHtml5Canvas::~CHtml5Canvas()
{
	
}

void CHtml5Canvas::StartMainLoop()
{
	std::cout << "Setting main loop..." << std::endl;
	emscripten_set_main_loop_arg(&CHtml5Canvas::MainLoopProcStub, this, 0, true);
}

void CHtml5Canvas::MainLoopProcStub(void* param)
{
	reinterpret_cast<CHtml5Canvas*>(param)->MainLoopProc();
}

void CHtml5Canvas::MainLoopProc()
{
	m_application->Update(1.f / 60.f);
	CHtml5GraphicDevice::GetInstance().Draw();
}

int main(int argc, char** argv)
{
	std::cout << "Starting Meleon HTML5..." << std::endl;
	CHtml5Canvas canvas;
	canvas.StartMainLoop();
	return 0;
}