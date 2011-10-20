#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <memory>
#include "ResourceManager.h"
#include "athena/Color.h"

namespace Athena
{
	class CMaterial;
	typedef std::tr1::shared_ptr<CMaterial> MaterialPtr;

	enum RENDER_TYPE
	{
        RENDER_ONLYCOLOR,
		RENDER_DIFFUSE,
		RENDER_ADDITIVE,
		RENDER_LIGHTMAPPED,
	};

	class CMaterial
	{
	public:
		virtual					~CMaterial();

		static MaterialPtr		Create();

		bool					operator <(const CMaterial&) const;

		bool					GetIsTransparent() const;
		void					SetIsTransparent(bool);

		RENDER_TYPE				GetRenderType() const;
		void					SetRenderType(RENDER_TYPE);

		TexturePtr				GetTexture(unsigned int) const;
		void					SetTexture(unsigned int, const TexturePtr&);

		CColor					GetColor() const;
		void					SetColor(const CColor&);

	protected:
		enum
		{
			MAX_TEXTURE_SLOTS = 8,
		};
								CMaterial();

		bool					m_isTransparent;
		RENDER_TYPE				m_renderType;

		TexturePtr				m_textures[MAX_TEXTURE_SLOTS];

		CColor					m_color;
	};
};

#endif
