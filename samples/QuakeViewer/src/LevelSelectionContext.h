#ifndef _LEVELSELECTIONCONTEXT_H_
#define _LEVELSELECTIONCONTEXT_H_

#include "AthenaEngine.h"
#include "ContextBase.h"

class CLevelSelectionContext : public CContextBase
{
public:
							CLevelSelectionContext(CPakFile* pakFile);
	virtual					~CLevelSelectionContext();

	void					Update(float);

	void					NotifyMouseMove(int, int) override;
	void					NotifyMouseDown();
	void					NotifyMouseUp();

private:
	enum
	{
		MAX_LEVELS = 4
	};

	Athena::TexturePtr		LoadTexture(const char*);

	void					OnShotButtonPress(unsigned int);

	CVector2				m_mousePosition;

	Athena::ViewportPtr		m_viewport;
	Athena::TexturePtr		m_levelShotTextures[MAX_LEVELS];

	static const char*		g_levelPaths[MAX_LEVELS];
	static const char*		g_levelShotTexturePaths[MAX_LEVELS];
};

#endif
