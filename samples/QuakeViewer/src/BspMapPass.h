#ifndef _BSPMAPPASS_H_
#define _BSPMAPPASS_H_

#include "PalleonEngine.h"
#include "BspMapTcMod.h"
#include <memory>

class CBspMapPass
{
public:
	enum TEXTURE_SOURCE
	{
		TEXTURE_SOURCE_DIFFUSE,
		TEXTURE_SOURCE_LIGHTMAP,
	};

									CBspMapPass();
	virtual							~CBspMapPass();

	void							Update(float);

	Palleon::TexturePtr				GetTexture() const;
	void							SetTexture(const Palleon::TexturePtr&);

	TEXTURE_SOURCE					GetTextureSource() const;
	void							SetTextureSource(TEXTURE_SOURCE);

	Palleon::TEXTURE_COMBINE_MODE	GetBlendingFunction() const;
	void							SetBlendingFunction(const Palleon::TEXTURE_COMBINE_MODE);

	const CMatrix4&					GetUvMatrix() const;

	void							AddTcMod(const BspMapTcModPtr&);

private:
	typedef std::vector<BspMapTcModPtr> TcModArray;

	CMatrix4						m_uvMatrix;

	Palleon::TexturePtr				m_texture;
	TcModArray						m_tcMods;

	TEXTURE_SOURCE					m_textureSource;
	Palleon::TEXTURE_COMBINE_MODE	m_blendingFunction;
};

typedef std::shared_ptr<CBspMapPass> BspMapPassPtr;

#endif
