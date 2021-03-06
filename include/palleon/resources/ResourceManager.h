#pragma once

#include <unordered_map>
#include <string>
#include <assert.h>
#include "Resource.h"
#include "TextureResource.h"
#include "Types.h"

namespace Palleon
{
	class CResourceManager
	{
	public:
		typedef std::unique_ptr<Framework::CStream> StreamPtr;

		virtual						~CResourceManager();
		
		static CResourceManager&	GetInstance();

		virtual std::string			MakeResourcePath(const std::string&) const = 0;
		virtual StreamPtr			MakeResourceStream(const std::string&) const = 0;

		ResourcePtr					GetResource(const std::string&) const;

		template <typename T>
		const T* GetResource(const std::string& fileName) const
		{
			auto resource = GetResource(fileName);
			assert(resource);
			auto specResource = std::dynamic_pointer_cast<T>(resource);
			assert(specResource);
			return specResource.get();
		}

		TexturePtr					GetTexture(const std::string&) const;

		template <typename T>
		void LoadResource(const std::string& name, const std::string& localPath = std::string())
		{
			std::string resourcePath = localPath.empty() ? name : localPath;
			unsigned int resId = MakeCrc(name);
			assert(m_resources.find(resId) == std::end(m_resources));
			auto resource = std::shared_ptr<T>(new T());
			auto inputStream = MakeResourceStream(resourcePath);
			resource->Load(*inputStream);
			m_resources[resId] = resource;
		}

		void						ReleaseResource(const std::string&);

	protected:
		typedef std::unordered_map<uint32, ResourcePtr> ResourceMap;

									CResourceManager();

		static uint32				MakeCrc(const std::string&);

		static CResourceManager*	m_instance;
		ResourceMap					m_resources;
	};
}
