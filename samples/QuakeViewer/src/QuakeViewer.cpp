#include <assert.h>
#include <math.h>
#include "QuakeViewer.h"
#include "PtrStream.h"
#include "QuakeEntityParser.h"
#include "LevelViewContext.h"
#include "LevelSelectionContext.h"

CQuakeViewer::CQuakeViewer()
: m_pakFile(nullptr)
, m_currentContext(nullptr)
, m_pendingContext(-1)
{
	m_globalPackage = Athena::CPackage::Create("global");

	std::string pakFilePath = Athena::CResourceManager::GetInstance().MakeResourcePath("pak0.pk3");
	m_pakFile = new CPakFile(pakFilePath.c_str());

	ChangeContext(QuakeViewer::CONTEXT_LEVEL_SELECTION, "");
//	ChangeContext(QuakeViewer::CONTEXT_LEVEL_VIEW, "maps/q3dm1.bsp");
}

CQuakeViewer::~CQuakeViewer()
{
	delete m_currentContext;
	delete m_pakFile;
}

Athena::CApplication* CreateApplication()
{
	return new CQuakeViewer();
}

void CQuakeViewer::ChangeContext(QuakeViewer::CONTEXT context, const char* param)
{
	if(m_currentContext)
	{
		delete m_currentContext;
		m_currentContext = nullptr;
	}

	switch(context)
	{
	case QuakeViewer::CONTEXT_LEVEL_SELECTION:
		m_currentContext = new CLevelSelectionContext(m_pakFile);
		break;
	case QuakeViewer::CONTEXT_LEVEL_VIEW:
		m_currentContext = new CLevelViewContext(m_pakFile, param);
		break;
	}

	m_currentContext->Finished.connect(boost::bind(&CQuakeViewer::OnContextFinished, this, _1, _2));
}

void CQuakeViewer::OnContextFinished(int nextContext, const char* param)
{
	assert(m_pendingContext == -1);
	m_pendingContext = nextContext;
	m_pendingContextParam = param;
}

void CQuakeViewer::Update(float dt)
{
	if(m_pendingContext != -1)
	{
		ChangeContext(static_cast<QuakeViewer::CONTEXT>(m_pendingContext), m_pendingContextParam.c_str());
		m_pendingContext = -1;
		m_pendingContextParam.clear();
	}

	if(m_currentContext)
	{
		m_currentContext->Update(dt);
	}
}

void CQuakeViewer::NotifyMouseMove(int x, int y)
{
	if(m_currentContext)
	{
		m_currentContext->NotifyMouseMove(x, y);
	}
}

void CQuakeViewer::NotifyMouseDown()
{
	if(m_currentContext)
	{
		m_currentContext->NotifyMouseDown();
	}
}

void CQuakeViewer::NotifyMouseUp()
{
	if(m_currentContext)
	{
		m_currentContext->NotifyMouseUp();
	}
}
