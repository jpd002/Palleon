#pragma once

#include <string>
#include "Resource.h"

namespace Palleon
{
	class CNinePatchDescriptor : public CResource
	{
	public:
						CNinePatchDescriptor();
		virtual			~CNinePatchDescriptor();

		void			Load(const char*) override;

		const char*		GetTextureName() const;

		unsigned int	GetWidth() const;
		unsigned int	GetHeight() const;

		unsigned int	GetLeftMargin() const;
		unsigned int	GetTopMargin() const;
		unsigned int	GetRightMargin() const;
		unsigned int	GetBottomMargin() const;

	private:
		unsigned int	m_width;
		unsigned int	m_height;

		unsigned int	m_leftMargin;
		unsigned int	m_topMargin;
		unsigned int	m_rightMargin;
		unsigned int	m_bottomMargin;

		std::string		m_textureName;
	};
};
