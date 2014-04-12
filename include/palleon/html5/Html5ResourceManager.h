#pragma once

#include "athena/resources/ResourceManager.h"

namespace Athena
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
