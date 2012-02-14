#ifndef _ATHENA_PACKAGE_H_
#define _ATHENA_PACKAGE_H_

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
			ITEM_INVALID = 0,
			ITEM_TEXTURE = 1,
			ITEM_FONTDESCRIPTOR = 2,
			ITEM_NINEPATCHDESCRIPTOR = 3,
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

#endif
