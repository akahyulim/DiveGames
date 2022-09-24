#pragma once
#include "Core/Object.h"
#include "Math/StringHash.h"
#include "IO/Log.h"
#include "Resource.h"

namespace Dive
{
	class Context;
	class FileStream;

	class ResourceCache : public Object
	{
		DIVE_OBJECT(ResourceCache, Object)

	public:
		explicit ResourceCache(Context* pContext);
		~ResourceCache() override;

		bool AddManualResource(Resource* pResource);

		template<class T> T* GetResource(const std::string& name);
		Resource* GetResource(StringHash type, const std::string& name);
		template<class T> void GetResources(std::vector<T*>& outReosurces);
		void GetResources(StringHash type, std::vector<Resource*>& outResources);

		template<class T> void RemoveResource(const std::string& name);
		void RemoveResource(StringHash type, const std::string& name);
		template<class T> void RemoveResources();
		void RemoveResources(StringHash type);
		void RemoveAllResources();

		bool AddResourceDir(const std::string& pathName);
		void RemoveResourceDir(const std::string& pathName);
		std::vector<std::string> GetResourceDirs() const { return m_ResourceDirs; }

	private:
		Resource* findResource(StringHash type, StringHash nameHash);
		std::string fixResourceName(const std::string& name) const;
		std::string fixResourceDirName(const std::string& name) const;
		FileStream* getFileStream(const std::string& name) const;

	private:
		using ResourceGroup = std::unordered_map<unsigned int, Resource*>;
		std::unordered_map<unsigned int, ResourceGroup> m_ResourceGroups;

		std::vector<std::string> m_ResourceDirs;
	};

	template<class T>
	void ResourceCache::RemoveResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		RemoveResource(type, name);
	}

	template<class T>
	void ResourceCache::RemoveResources()
	{
		StringHash type = T::GetTypeStatic();
		RemoveResources(type);
	}

	template<class T>
	T* ResourceCache::GetResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		// 왜 static_cast로 변환했을까...?
		return dynamic_cast<T*>(GetResource(type, name));
	}

	template<class T>
	void ResourceCache::GetResources(std::vector<T*>& outResources)
	{
		std::vector<Resource*> resources;

		StringHash type = T::GetTypeStatic();
		GetResources(type, resources);

		outResources.resize(resources.size());
		for (unsigned int i = 0; i < (unsigned int)resources.size(); ++i)
		{
			outResources[i] = dynamic_cast<T*>(resources[i]);
		}
	}
}