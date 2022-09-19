#pragma once
#include "Core/Object.h"
#include "Math/StringHash.h"
#include "IO/Log.h"
#include "Resource.h"

namespace Dive
{
	class Context;

	class ResourceCache : public Object
	{
		DIVE_OBJECT(ResourceCache, Object)

	public:
		explicit ResourceCache(Context* pContext);
		~ResourceCache() override;

		// ���� ���� ���ҽ�(manual resource?) ���.
		bool AddResource(Resource* pResource);

		// �ش� Ÿ�԰� �̸��� ���ҽ��� �����Ѵ�.
		template<class T> void RemoveResource(const std::string& name);
		// �ش� Ÿ�԰� �̸��� ���ҽ��� �����Ѵ�.
		void RemoveResource(StringHash type, const std::string& name);
		// �ش� Ÿ���� ���ҽ����� ��� �����Ѵ�.
		template<class T> void RemoveResources();
		// �ش� Ÿ���� ���ҽ����� ��� �����Ѵ�.
		void RemoveResources(StringHash type);
		// ��� ���ҽ��� �����Ѵ�.
		void RemoveAllResources();

		// �ش� Ÿ�԰� �̸��� ���ҽ��� ���Ϲ޴´�. �������� �ʴ´ٸ� ���� �� ��ϱ��� �����Ѵ�.
		template<class T> T* GetResource(const std::string& name);
		// �ش� Ÿ�԰� �̸��� ���ҽ��� ���Ϲ޴´�. �������� �ʴ´ٸ� ���� �� ��ϱ��� �����Ѵ�.
		Resource* GetResource(StringHash type, const std::string& name);
		// �ش� Ÿ���� ���ҽ����� ������ ���Ϳ� �����Ѵ�.
		template<class T> void GetResources(std::vector<T*>& outReosurces);
		// �ش� Ÿ���� ���ҽ����� ������ ���Ϳ� �����Ѵ�.
		void GetResources(StringHash type, std::vector<Resource*>& outResources);

	private:
		Resource* findResource(StringHash type, StringHash nameHash);

	private:
		using ResourceGroup = std::unordered_map<unsigned int, Resource*>;
		std::unordered_map<unsigned int, ResourceGroup> m_ResourceGroups;
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
		// �� static_cast�� ��ȯ������...?
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