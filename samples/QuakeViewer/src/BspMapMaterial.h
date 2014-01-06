#ifndef _BSPMAPMATERIAL_H_
#define _BSPMAPMATERIAL_H_

#include "BspMapPass.h"

class CBspMapMaterial
{
public:
	enum
	{
		MAX_PASS = 6
	};

						CBspMapMaterial();
	virtual				~CBspMapMaterial();

	void				Update(float dt);

	void				AddPass(const BspMapPassPtr&);

	BspMapPassPtr		GetPass(unsigned int) const;
	unsigned int		GetPassCount() const;

	bool				GetIsSky() const;
	void				SetIsSky(bool);

private:
	typedef std::vector<BspMapPassPtr> PassArray;

	bool				m_isSky;
	PassArray			m_passes;
};

typedef std::shared_ptr<CBspMapMaterial> BspMapMaterialPtr;

#endif
