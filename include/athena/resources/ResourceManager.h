#pragma once

#include <unordered_map>
#include <assert.h>
#include "Resource.h"
#include "TextureResource.h"
#include "Types.h"

namespace Athena
{
	class CResourceManager
	{
	public:
		virtual						~CResourceManager();
		
		static CResourceManager&	GetInstance();

		virtual std::string			MakeResourcePath(const char*) const = 0;
		
		ResourcePtr					GetResource(const char*) const;

		template <typename T>
		const T* GetResource(const char* fileName) const
		{
			auto resource = GetResource(fileName);
			assert(resource);
			auto specResource = std::dynamic_pointer_cast<T>(resource);
			assert(specResource);
			return specResource.get();
		}

		TexturePtr					GetTexture(const char*) const;

		template <typename T>
		void LoadResource(const char* name, const char* localPath = nullptr)
		{
			if(!localPath) localPath = name;
			unsigned int resId = MakeCrc(name);
			auto path = MakeResourcePath(localPath);
			assert(m_resources.find(resId) == std::end(m_resources));
			auto resource = std::shared_ptr<T>(new T());
			resource->Load(path.c_str());
			m_resources[resId] = resource;
		}

		void						ReleaseResource(const char*);

	protected:
		typedef std::unordered_map<uint32, ResourcePtr> ResourceMap;

									CResourceManager();

		static uint32				MakeCrc(const char*);

		static CResourceManager*	m_instance;
		ResourceMap					m_resources;
	};
}
