#ifndef _QUAKEVIEWER_H_
#define _QUAKEVIEWER_H_

#include "AthenaEngine.h"
#include "ContextBase.h"
#include "QuakeViewerDefs.h"

class CQuakeViewer : public Athena::CApplication
{
public:
								CQuakeViewer();
	virtual						~CQuakeViewer();

	virtual void				Update(float);

	virtual void				NotifyMouseMove(int, int) override;
	virtual void				NotifyMouseDown();
	virtual void				NotifyMouseUp();

private:
	void						ChangeContext(QuakeViewer::CONTEXT, const char*);

	void						OnContextFinished(int, const char*);

	Athena::PackagePtr			m_globalPackage;

	CPakFile*					m_pakFile;
	CContextBase*				m_currentContext;
	int							m_pendingContext;
	std::string					m_pendingContextParam;
};

#endif
