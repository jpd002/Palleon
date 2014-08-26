#pragma once

#include "palleon/ILayoutable.h"
#include "layout/LayoutObject.h"

namespace Palleon
{
	class CLayoutNode : public Framework::CLayoutObject
	{
	public:
								CLayoutNode(unsigned int, unsigned int, unsigned int, unsigned int, const LayoutablePtr&);
		virtual					~CLayoutNode();

		unsigned int			GetPreferredWidth() override;
		unsigned int			GetPreferredHeight() override;
		void					RefreshGeometry() override;

	private:
		unsigned int			m_prefWidth;
		unsigned int			m_prefHeight;
		LayoutablePtr			m_layoutable;
	};
}
