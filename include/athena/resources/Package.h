#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Athena
{
	class CPackage;
	typedef std::shared_ptr<CPackage> PackagePtr;

	class CPackage
	{
	public:
								CPackage(const char*);
		virtual					~CPackage();

		static PackagePtr		Create(const char* name);

	protected:
		enum ITEM_TYPE
		{
			ITEM_INVALID,
			ITEM_TEXTURE,
			ITEM_FONTDESCRIPTOR,
			ITEM_NINEPATCHDESCRIPTOR,
			ITEM_EMITTERDESCRIPTOR,
			ITEM_SCENEDESCRIPTOR
		};

		struct ITEM
		{
			std::string		name;
			ITEM_TYPE		type;
		};

		typedef std::vector<ITEM> ItemArray;

		void					LoadDefinition();

		void					LoadItems();
		void					ReleaseItems();

		std::string				m_name;
		ItemArray				m_items;
	};
};
