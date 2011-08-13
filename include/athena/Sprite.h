#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "Mesh.h"

namespace Athena
{
	class CSprite;
	typedef std::tr1::shared_ptr<CSprite> SpritePtr;

	class CSprite : public CMesh
	{
	public:
		virtual             ~CSprite();

		static SpritePtr    Create();
	    
		virtual void        Update(float dt);
		
		void                SetSize(const CVector2&);
		
		void                SetHotspot(const CVector2&);
		
		void                SetTextureExtends(float, float);
		
	protected:
							CSprite();
	    
		void				UpdateVertices();
		void				UpdateIndices();

		enum STATUS_FLAGS
		{
			STATUS_VERTEXBUFFER_DIRTY = 0x01,
		};

		CVector2            m_size;
		CVector2            m_hotspot;
		
		uint32				m_status;
	};
}

#endif
