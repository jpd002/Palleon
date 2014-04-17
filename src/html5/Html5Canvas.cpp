#include <iostream>
#include <emscripten.h>
#include "palleon/html5/Html5Canvas.h"
#include "palleon/html5/Html5GraphicDevice.h"
#include "palleon/html5/Html5ResourceManager.h"

using namespace Palleon;

CHtml5Canvas::CHtml5Canvas()
{
	CHtml5ResourceManager::CreateInstance();
	CHtml5GraphicDevice::CreateInstance();

	m_application = CreateApplication();
}

CHtml5Canvas::~CHtml5Canvas()
{

}

void CHtml5Canvas::StartMainLoop()
{
	std::cout << "Starting main loop..." << std::endl;
	m_currentTime = emscripten_get_now();
	emscripten_set_mousemove_callback("canvas", this, 1, &CHtml5Canvas::MouseMoveCallbackStub);
	emscripten_set_mousedown_callback("canvas", this, 1, &CHtml5Canvas::MouseDownCallbackStub);
	emscripten_set_mouseup_callback("canvas", this, 1, &CHtml5Canvas::MouseUpCallbackStub);
	emscripten_set_main_loop_arg(&CHtml5Canvas::MainLoopProcStub, this, 0, true);
}

void CHtml5Canvas::MainLoopProcStub(void* param)
{
	reinterpret_cast<CHtml5Canvas*>(param)->MainLoopProc();
}

int CHtml5Canvas::MouseMoveCallbackStub(int eventType, const EmscriptenMouseEvent* mouseEvent, void* param)
{
	return reinterpret_cast<CHtml5Canvas*>(param)->MouseMoveCallback(eventType, mouseEvent);
}

int CHtml5Canvas::MouseDownCallbackStub(int eventType, const EmscriptenMouseEvent* mouseEvent, void* param)
{
	return reinterpret_cast<CHtml5Canvas*>(param)->MouseDownCallback(eventType, mouseEvent);
}

int CHtml5Canvas::MouseUpCallbackStub(int eventType, const EmscriptenMouseEvent* mouseEvent, void* param)
{
	return reinterpret_cast<CHtml5Canvas*>(param)->MouseUpCallback(eventType, mouseEvent);
}

void CHtml5Canvas::MainLoopProc()
{
	auto nextTime = emscripten_get_now();
	auto deltaTime = (nextTime - m_currentTime) / 1000.f;
	m_currentTime = nextTime;
	
	m_application->Update(static_cast<float>(deltaTime));
	CHtml5GraphicDevice::GetInstance().Draw();
	
	m_currentFrameCount++;
	m_frameCounterTime += deltaTime;
	if(m_frameCounterTime > 1)
	{
		float frameRate = static_cast<float>(m_currentFrameCount) / m_frameCounterTime;
		m_frameCounterTime = 0;
		m_currentFrameCount = 0;
		static_cast<CGlEsGraphicDevice&>(CGraphicDevice::GetInstance()).SetFrameRate(frameRate);
	}
}

int CHtml5Canvas::MouseMoveCallback(int eventType, const EmscriptenMouseEvent* mouseEvent)
{
	m_application->NotifyMouseMove(mouseEvent->canvasX, mouseEvent->canvasY);
	return 0;
}

int CHtml5Canvas::MouseDownCallback(int eventType, const EmscriptenMouseEvent* mouseEvent)
{
	m_application->NotifyMouseDown();
	return 0;
}

int CHtml5Canvas::MouseUpCallback(int eventType, const EmscriptenMouseEvent* mouseEvent)
{
	m_application->NotifyMouseUp();
	return 0;
}

int main(int argc, char** argv)
{
	std::cout << "Starting Palleon HTML5..." << std::endl;
	CHtml5Canvas canvas;
	canvas.StartMainLoop();
	return 0;
}
