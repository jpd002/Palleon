#ifndef _BSPMAPRESOURCEPROVIDER_H_
#define _BSPMAPRESOURCEPROVIDER_H_

#include "PalleonEngine.h"
#include <map>
#include "PakFile.h"
#include "BspFile.h"
#include "BspMapMaterial.h"
#include "QuakeShader.h"

class CBspMapResourceProvider
{
public:
							CBspMapResourceProvider();
	virtual					~CBspMapResourceProvider();

	void					LoadResources(const CBspFile&, CPakFile&);
	void					UpdateMaterials(float);

	BspMapMaterialPtr		GetMaterial(uint32) const;

	Palleon::TexturePtr		GetTexture(const char*) const;
	Palleon::TexturePtr		GetLightMap(uint32) const;

private:
	typedef std::map<std::string, Palleon::TexturePtr> TextureMap;
	typedef std::vector<BspMapMaterialPtr> MaterialMap;
	typedef std::map<uint32, Palleon::TexturePtr> LightMapMap;
	typedef std::map<std::string, QUAKE_SHADER> ShaderMap;

	void					LoadTextures(const CBspFile&, CPakFile&);
	void					LoadLightMaps(const CBspFile&);
	void					LoadShaders(CPakFile&);

	void					LoadTexture(const char*, CPakFile&);

	TextureMap				m_textures;
	MaterialMap				m_materials;
	LightMapMap				m_lightMaps;
	ShaderMap				m_shaders;
};

#endif
