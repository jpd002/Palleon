#include "BspMapResourceProvider.h"
#include "PalleonEngine.h"
#include "QuakeShaderParser.h"

CBspMapResourceProvider::CBspMapResourceProvider()
{

}

CBspMapResourceProvider::~CBspMapResourceProvider()
{

}

void CBspMapResourceProvider::LoadResources(const CBspFile& bspFile, CPakFile& pakFile)
{
	LoadShaders(pakFile);
	LoadTextures(bspFile, pakFile);
	LoadLightMaps(bspFile);
}

void CBspMapResourceProvider::UpdateMaterials(float dt)
{
	for(uint32 i = 0; i < m_materials.size(); i++)
	{
		BspMapMaterialPtr material = m_materials[i];
		if(!material) continue;
		material->Update(dt);
	}
}

void CBspMapResourceProvider::LoadShaders(CPakFile& pakFile)
{
	CPakFile::FileNameList shaderFileNames = pakFile.GetFileNamesMatching(".shader");
	for(CPakFile::FileNameList::const_iterator fileNameIterator(shaderFileNames.begin());
		fileNameIterator != shaderFileNames.end(); fileNameIterator++)
	{
		const std::string& shaderFileName(*fileNameIterator);

		uint8* fileData(NULL);
		uint32 fileSize(0);
		if(!pakFile.ReadFile(shaderFileName.c_str(), &fileData, &fileSize))
		{
			continue;
		}

		std::string shaderString(fileData, fileData + fileSize);

		delete fileData;

		QuakeShaderList shaders = CQuakeShaderParser::ParseShaders(shaderString.c_str());
		for(QuakeShaderList::const_iterator shaderIterator(shaders.begin());
			shaderIterator != shaders.end(); shaderIterator++)
		{
			m_shaders[shaderIterator->name] = *shaderIterator;
		}
	}
}

void CBspMapResourceProvider::LoadTextures(const CBspFile& bspFile, CPakFile& pakFile)
{
	const Bsp::TextureArray& textures(bspFile.GetTextures());
	m_materials.resize(textures.size());
	for(uint32 i = 0; i < textures.size(); i++)
	{
		const Bsp::TEXTURE& texture(textures[i]);

		auto resultMaterial = BspMapMaterialPtr(new CBspMapMaterial());
		m_materials[i] = resultMaterial;

		ShaderMap::const_iterator shaderIterator = m_shaders.find(texture.name);
		if(shaderIterator == m_shaders.end())
		{
			//Create a basic material with this texture
			std::string fullName;
			if(!pakFile.TryCompleteFileName(texture.name, fullName))
			{
				continue;
			}

			LoadTexture(fullName.c_str(), pakFile);

			{
				auto pass = BspMapPassPtr(new CBspMapPass());
				pass->SetTexture(GetTexture(fullName.c_str()));
				pass->SetTextureSource(CBspMapPass::TEXTURE_SOURCE_DIFFUSE);
				pass->SetBlendingFunction(Palleon::TEXTURE_COMBINE_MODULATE);
				resultMaterial->AddPass(pass);
			}

			{
				auto pass = BspMapPassPtr(new CBspMapPass());
				pass->SetTextureSource(CBspMapPass::TEXTURE_SOURCE_LIGHTMAP);
				pass->SetBlendingFunction(Palleon::TEXTURE_COMBINE_MODULATE);
				resultMaterial->AddPass(pass);
			}
		}
		else
		{
			const QUAKE_SHADER& shader = shaderIterator->second;

			resultMaterial->SetIsSky(shader.isSky);

			for(QuakeShaderPassArray::const_iterator passIterator(shader.passes.begin());
				passIterator != shader.passes.end(); passIterator++)
			{
				const QUAKE_SHADER_PASS& passData(*passIterator);
				if(passData.mapName.length() < 4) continue;

				auto pass = BspMapPassPtr(new CBspMapPass());

				{
					Palleon::TEXTURE_COMBINE_MODE blendingFunction = Palleon::TEXTURE_COMBINE_MODULATE;
					switch(passData.blendFunc)
					{
					case QUAKE_SHADER_BLEND_BLEND:
						blendingFunction = Palleon::TEXTURE_COMBINE_LERP;
						break;
					case QUAKE_SHADER_BLEND_ADD:
						blendingFunction = Palleon::TEXTURE_COMBINE_ADD;
						break;
					case QUAKE_SHADER_BLEND_FILTER:
						blendingFunction = Palleon::TEXTURE_COMBINE_MODULATE;
						break;
					}
					pass->SetBlendingFunction(blendingFunction);
				}

				if(passData.mapName == "$lightmap")
				{
					pass->SetTextureSource(CBspMapPass::TEXTURE_SOURCE_LIGHTMAP);
				}
				else
				{
					std::string fileName(passData.mapName.begin(), passData.mapName.begin() + passData.mapName.length() - 4);
					
					std::string fullName;
					if(!pakFile.TryCompleteFileName(fileName.c_str(), fullName))
					{
						continue;
					}

					LoadTexture(fullName.c_str(), pakFile);
					pass->SetTexture(GetTexture(fullName.c_str()));
					pass->SetTextureSource(CBspMapPass::TEXTURE_SOURCE_DIFFUSE);
				}

				for(unsigned int i = 0; i < passData.tcMods.size(); i++)
				{
					const QUAKE_SHADER_TCMOD& tcMod(passData.tcMods[i]);
					BspMapTcModPtr result;
					switch(tcMod.type)
					{
					case QUAKE_SHADER_TCMOD_SCROLL:
						result = BspMapTcModPtr(new CBspMapTcMod_Scroll(
									tcMod.params[0],
									tcMod.params[1]));
						break;
					case QUAKE_SHADER_TCMOD_SCALE:
						result = BspMapTcModPtr(new CBspMapTcMod_Scale(
									tcMod.params[0],
									tcMod.params[1]));
						break;
					case QUAKE_SHADER_TCMOD_ROTATE:
						result = BspMapTcModPtr(new CBspMapTcMod_Rotate(
									tcMod.params[0]));
						break;
					case QUAKE_SHADER_TCMOD_TURB:
						result = BspMapTcModPtr(new CBspMapTcMod_Turb(
									tcMod.params[1],
									tcMod.params[3]));
						break;
					case QUAKE_SHADER_TCMOD_STRETCH:
						{
							BSPMAPWAVEPARAMS wave;
							wave.type		= BSPMAPWAVEPARAMS::WAVE_SIN;
							wave.base		= tcMod.params[0];
							wave.amplitude	= tcMod.params[1];
							wave.phase		= tcMod.params[2];
							wave.freq		= tcMod.params[3];
							result = BspMapTcModPtr(new CBspMapTcMod_Stretch(wave));
						}
						break;
					default:
						continue;
						break;
					}
					pass->AddTcMod(result);
				}

				resultMaterial->AddPass(pass);

				if(resultMaterial->GetPassCount() == CBspMapMaterial::MAX_PASS)
				{
					break;
				}
			}
		}
	}
}

void CBspMapResourceProvider::LoadTexture(const char* texturePath, CPakFile& pakFile)
{
	if(m_textures.find(texturePath) != m_textures.end()) return;

	uint8* fileData(NULL);
	uint32 fileSize(0);
	if(!pakFile.ReadFile(texturePath, &fileData, &fileSize))
	{
		return;
	}

	auto result = Palleon::CTextureLoader::CreateTextureFromMemory(fileData, fileSize);
	delete fileData;

	m_textures[texturePath] = result;
}

void CBspMapResourceProvider::LoadLightMaps(const CBspFile& bspFile)
{
	const auto& lightMaps(bspFile.GetLightMaps());
	for(uint32 i = 0; i < lightMaps.size(); i++)
	{
		const auto& lightMap(lightMaps[i]);
		auto result = Palleon::CGraphicDevice::GetInstance().CreateTexture(Palleon::TEXTURE_FORMAT_RGB888, 128, 128, 1);
		result->Update(0, lightMap.colors);
		m_lightMaps[i] = result;
	}
}

BspMapMaterialPtr CBspMapResourceProvider::GetMaterial(uint32 textureId) const
{
	return m_materials[textureId];
}

Palleon::TexturePtr CBspMapResourceProvider::GetTexture(const char* textureName) const
{
	TextureMap::const_iterator textureIterator = m_textures.find(textureName);
	if(textureIterator != m_textures.end())
	{
		return textureIterator->second;
	}
	else
	{
		return Palleon::TexturePtr();
	}
}

Palleon::TexturePtr CBspMapResourceProvider::GetLightMap(uint32 lightMapIndex) const
{
	LightMapMap::const_iterator lightMapIterator = m_lightMaps.find(lightMapIndex);
	if(lightMapIterator != m_lightMaps.end())
	{
		return lightMapIterator->second;
	}
	else
	{
		return Palleon::TexturePtr();
	}
}
