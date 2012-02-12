#ifndef _LABEL_H_
#define _LABEL_H_

#include <string>
#include "Mesh.h"
#include "FontDescriptor.h"
#include "ResourceManager.h"
#include "ILayoutable.h"

namespace Athena
{
	class CLabel;
	typedef std::tr1::shared_ptr<CLabel> LabelPtr;

	class CLabel : public CMesh, public ILayoutable
	{
	public:
		enum HORIZONTAL_ALIGNMENT
		{
			HORIZONTAL_ALIGNMENT_LEFT,
			HORIZONTAL_ALIGNMENT_CENTER,
			HORIZONTAL_ALIGNMENT_RIGHT,
		};

		enum VERTICAL_ALIGNMENT
		{
			VERTICAL_ALIGNMENT_TOP,
			VERTICAL_ALIGNMENT_CENTER,
			VERTICAL_ALIGNMENT_BOTTOM,
		};

		virtual					~CLabel();
		
		static LabelPtr			Create();

		void					SetFont(const CFontDescriptor*);
		void					SetText(const char*);

		HORIZONTAL_ALIGNMENT	GetHorizontalAlignment() const;
		void					SetHorizontalAlignment(HORIZONTAL_ALIGNMENT);

		VERTICAL_ALIGNMENT		GetVerticalAlignment() const;
		void					SetVerticalAlignment(VERTICAL_ALIGNMENT);

		virtual void			SetPosition(const CVector3&);

		void					SetSize(const CVector2&);

		void					SetTextScale(const CVector2&);

		void					Update(float);
		
	protected:
		typedef std::vector<float> FloatArray;

		struct TEXTPOSINFO
		{
			FloatArray			linePosX;
			float				posY;
		};

								CLabel();

		unsigned int			GetLineCount() const;
		FloatArray				GetLineWidths() const;
		float					GetTextHeight() const;
		CVector2				GetTextExtents() const;
		TEXTPOSINFO				GetTextPosition() const;

		void					BuildVertexBuffer();
		
		const CFontDescriptor*	m_font;
		std::string				m_text;
		CVector2				m_textScale;

		HORIZONTAL_ALIGNMENT	m_horizontalAlignment;
		VERTICAL_ALIGNMENT		m_verticalAlignment;
		CVector2				m_size;

		bool					m_dirty;
		uint32					m_charCount;
	};
}

#endif
