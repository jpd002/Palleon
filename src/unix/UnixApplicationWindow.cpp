#include <cstdio>
#include <cassert>
#include "palleon/unix/UnixApplicationWindow.h"
#include "palleon/unix/UnixResourceManager.h"

using namespace Palleon;

CUnixApplicationWindow::CUnixApplicationWindow()
{
	m_connection = xcb_connect(nullptr, nullptr);
	assert(xcb_connection_has_error(m_connection) == 0);
	
	const auto setup = xcb_get_setup(m_connection);
	auto screenIterator = xcb_setup_roots_iterator(setup);
	auto screen = screenIterator.data;

	m_window = xcb_generate_id(m_connection);
	uint32_t valueMask = XCB_CW_EVENT_MASK;
	uint32_t eventMask = XCB_EVENT_MASK_EXPOSURE;
	xcb_create_window(
		m_connection, XCB_COPY_FROM_PARENT, m_window, screen->root,
		0, 0, 640, 480, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
		valueMask, &eventMask);

	xcb_map_window(m_connection, m_window);
	xcb_flush(m_connection);

	CUnixResourceManager::CreateInstance();

	m_application = CreateApplication();
}

CUnixApplicationWindow::~CUnixApplicationWindow()
{
	delete m_application;
	CUnixResourceManager::DestroyInstance();
	xcb_disconnect(m_connection);
}

void CUnixApplicationWindow::Loop()
{
	while(1)
	{
		auto event = xcb_wait_for_event(m_connection);
		if(event == nullptr) break;
		printf("Event: %d\n", event->response_type);
	}

	printf("Done\n");
}

