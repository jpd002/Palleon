#ifndef _BSPMAPPASS_H_
#define _BSPMAPPASS_H_

#include "AthenaEngine.h"
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

	Athena::TexturePtr				GetTexture() const;
	void							SetTexture(const Athena::TexturePtr&);

	TEXTURE_SOURCE					GetTextureSource() const;
	void							SetTextureSource(TEXTURE_SOURCE);

	Athena::TEXTURE_COMBINE_MODE	GetBlendingFunction() const;
	void							SetBlendingFunction(const Athena::TEXTURE_COMBINE_MODE);

	const CMatrix4&					GetUvMatrix() const;

	void							AddTcMod(const BspMapTcModPtr&);

private:
	typedef std::vector<BspMapTcModPtr> TcModArray;

	CMatrix4						m_uvMatrix;

	Athena::TexturePtr				m_texture;
	TcModArray						m_tcMods;

	TEXTURE_SOURCE					m_textureSource;
	Athena::TEXTURE_COMBINE_MODE	m_blendingFunction;
};

typedef std::shared_ptr<CBspMapPass> BspMapPassPtr;

#endif
