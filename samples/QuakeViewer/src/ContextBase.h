#ifndef _CONTEXTBASE_H_
#define _CONTEXTBASE_H_

#include <boost/signals2.hpp>
#include "PakFile.h"

class CContextBase
{
public:
								CContextBase(CPakFile*);
	virtual						~CContextBase();

	virtual void				Update(float);

	virtual void				NotifyMouseMove(unsigned int, unsigned int);
	virtual void				NotifyMouseDown();
	virtual void				NotifyMouseUp();

	boost::signals2::signal<void (int, const char*)> Finished;

protected:
	CPakFile*					m_pakFile;
};

#endif
