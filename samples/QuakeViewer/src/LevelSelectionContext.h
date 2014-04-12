#ifndef _LEVELSELECTIONCONTEXT_H_
#define _LEVELSELECTIONCONTEXT_H_

#include "PalleonEngine.h"
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

	Palleon::TexturePtr		LoadTexture(const char*);

	void					OnShotButtonPress(unsigned int);

	CVector2				m_mousePosition;

	Palleon::ViewportPtr		m_viewport;
	Palleon::TexturePtr		m_levelShotTextures[MAX_LEVELS];

	static const char*		g_levelPaths[MAX_LEVELS];
	static const char*		g_levelShotTexturePaths[MAX_LEVELS];
};

#endif
