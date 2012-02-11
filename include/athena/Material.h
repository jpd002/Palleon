
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <memory>
#include "ResourceManager.h"
#include "athena/Color.h"
#include "athena/Matrix4.h"

namespace Athena
{
	class CMaterial;
	typedef std::tr1::shared_ptr<CMaterial> MaterialPtr;

	enum CULLING_MODE
	{
		CULLING_NONE,
		CULLING_CCW,
		CULLING_CW,
		CULLING_MODE_MAX
	};

	enum ALPHA_BLENDING_MODE
	{
		ALPHA_BLENDING_NONE,
		ALPHA_BLENDING_MODE_MAX
	};

	enum TEXTURE_COORD_SOURCE
	{
		TEXTURE_COORD_UV0,
		TEXTURE_COORD_UV1,
		TEXTURE_COORD_CUBE_POS,
		TEXTURE_COORD_CUBE_REFLECT,
		TEXTURE_COORD_SOURCE_MAX
	};

	enum TEXTURE_COMBINE_MODE
	{
		TEXTURE_COMBINE_MODULATE,
		TEXTURE_COMBINE_LERP,
		TEXTURE_COMBINE_ADD,
		TEXTURE_COMBINE_MODE_MAX
	};

	class CMaterial
	{
	public:
		enum
		{
			MAX_TEXTURE_SLOTS = 8,
		};

		virtual					~CMaterial();

		static MaterialPtr		Create();

		bool					operator <(const CMaterial&) const;

		bool					GetIsTransparent() const;
		void					SetIsTransparent(bool);

		CULLING_MODE			GetCullingMode() const;
		void					SetCullingMode(CULLING_MODE);

		TexturePtr				GetTexture(unsigned int) const;
		void					SetTexture(unsigned int, const TexturePtr&);

		const CMatrix4&			GetTextureMatrix(unsigned int) const;
		void					SetTextureMatrix(unsigned int, const CMatrix4&);

		TEXTURE_COORD_SOURCE	GetTextureCoordSource(unsigned int) const;
		void					SetTextureCoordSource(unsigned int, TEXTURE_COORD_SOURCE);

		TEXTURE_COMBINE_MODE	GetTextureCombineMode(unsigned int) const;
		void					SetTextureCombineMode(unsigned int, TEXTURE_COMBINE_MODE);

		CColor					GetColor() const;
		void					SetColor(const CColor&);

	protected:
								CMaterial();

		bool					m_isTransparent;
		CULLING_MODE			m_cullingMode;

		TexturePtr				m_textures[MAX_TEXTURE_SLOTS];
		CMatrix4				m_textureMatrices[MAX_TEXTURE_SLOTS];
		TEXTURE_COORD_SOURCE	m_textureCoordSources[MAX_TEXTURE_SLOTS];
		TEXTURE_COMBINE_MODE	m_textureCombineModes[MAX_TEXTURE_SLOTS];

		CColor					m_color;
	};
};

#endif
