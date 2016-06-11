#pragma once

#include "palleon/resources/ResourceManager.h"

namespace Palleon
{
	class CUnixResourceManager : public CResourceManager
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();

		std::string MakeResourcePath(const std::string&) const override;
		StreamPtr   MakeResourceStream(const std::string&) const override;

	private:
		        CUnixResourceManager() = default;
		virtual ~CUnixResourceManager() = default;
	};
}
