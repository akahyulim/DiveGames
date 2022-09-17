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

		// �ش� Ÿ�԰� �̸��� ���ҽ��� ���Ϲ޴´�. �������� �ʴ´ٸ� ���� �� ��ϱ��� �����Ѵ�.
		template<class T> T* GetResource(const std::string& name);
		// �ش� Ÿ�԰� �̸��� ���ҽ��� ���Ϲ޴´�. �������� �ʴ´ٸ� ���� �� ��ϱ��� �����Ѵ�.
		Resource* GetResource(StringHash type, const std::string& name);
		// �ش� Ÿ���� ���ҽ����� ������ ���Ϳ� �����Ѵ�.
		void GetResources(StringHash type, std::vector<Resource*>& outResources);

		// �ش� Ÿ�԰� �̸��� ���ҽ��� �����Ѵ�.
		template<class T> void ReleaseResource(const std::string& name);
		// �ش� Ÿ�԰� �̸��� ���ҽ��� �����Ѵ�.
		void ReleaseResource(StringHash type, const std::string& name);
		// �ش� Ÿ���� ���ҽ����� ��� �����Ѵ�.
		void ReleaseResources(StringHash type);
		// ��� ���ҽ��� �����Ѵ�.
		void ReleaseAllResources();

	private:
		Resource* findResource(StringHash type, StringHash nameHash);

	private:
		using ResourceGroup = std::unordered_map<unsigned int, Resource*>;
		std::unordered_map<unsigned int, ResourceGroup> m_ResourceGroups;
	};
	
	template<class T>
	T* ResourceCache::GetResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		// �� static_cast�� ��ȯ������...?
		return dynamic_cast<T*>(GetResource(type, name));
	}

	template<class T>
	void ResourceCache::ReleaseResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		ReleaseResource(type, name);
	}
}