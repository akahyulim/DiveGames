#pragma once
#include "Core/Object.h"
#include "Math/StringHash.h"
#include "Resource.h"

namespace Dive
{
	class Context;
	class FileStream;

	// 府家胶 包府 努贰胶.
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

		template<class T> T* GetExistingResource(const std::string& name);
		Resource* GetExistingResource(StringHash type, const std::string& name);

		template<class T> bool IsExistingResource(const std::string& name);
		bool IsExistingResource(StringHash type, const std::string& name);

		template<class T> void RemoveResource(const std::string& name);
		void RemoveResource(StringHash type, const std::string& name);
		template<class T> void RemoveSpecificTypeResources();
		void RemoveSpecificTypeResources(StringHash type);
		void RemoveAllResources();

	private:
		Resource* findResource(StringHash type, StringHash nameHash);

	private:
		using ResourceGroup = std::unordered_map<uint32_t, Resource*>;
		std::unordered_map<uint32_t, ResourceGroup> m_ResourceGroups;
	};

	template<class T>
	void ResourceCache::RemoveResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		RemoveResource(type, name);
	}

	template<class T>
	void ResourceCache::RemoveSpecificTypeResources()
	{
		StringHash type = T::GetTypeStatic();
		RemoveSpecificTypeResources(type);
	}

	template<class T>
	T* ResourceCache::GetResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		return dynamic_cast<T*>(GetResource(type, name));
	}

	template<class T>
	void ResourceCache::GetResources(std::vector<T*>& outResources)
	{
		std::vector<Resource*> resources;

		StringHash type = T::GetTypeStatic();
		GetResources(type, resources);

		outResources.resize(resources.size());
		for (uint32_t i = 0; i < static_cast<uint32_t>(resources.size()); ++i)
		{
			outResources[i] = dynamic_cast<T*>(resources[i]);
		}
	}

	template<class T> 
	T* ResourceCache::GetExistingResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		return dynamic_cast<T*>(GetExistingResource(type, name));
	}

	template<class T>
	bool ResourceCache::IsExistingResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		return IsExistingResource(type, name);
	}
}