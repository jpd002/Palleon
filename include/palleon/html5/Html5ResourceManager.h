#pragma once

#include "../resources/ResourceManager.h"

namespace Palleon
{
	class CHtml5ResourceManager : public CResourceManager
	{
	public:
		static void					CreateInstance();
		static void					DestroyInstance();
		
		virtual std::string			MakeResourcePath(const std::string&) const override;
		
	protected:
									CHtml5ResourceManager();
		virtual						~CHtml5ResourceManager();
	};
}
