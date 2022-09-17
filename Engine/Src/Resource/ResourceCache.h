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

		// 개별 생성 리소스(manual resource?) 등록.
		bool AddResource(Resource* pResource);

		// 해당 타입과 이름의 리소스를 리턴받는다. 존재하지 않는다면 생성 후 등록까지 수행한다.
		template<class T> T* GetResource(const std::string& name);
		// 해당 타입과 이름의 리소스를 리턴받는다. 존재하지 않는다면 생성 후 등록까지 수행한다.
		Resource* GetResource(StringHash type, const std::string& name);
		// 해당 타입의 리소스들을 전달한 벡터에 저장한다.
		void GetResources(StringHash type, std::vector<Resource*>& outResources);

		// 해당 타입과 이름의 리소스를 제거한다.
		template<class T> void ReleaseResource(const std::string& name);
		// 해당 타입과 이름의 리소스를 제거한다.
		void ReleaseResource(StringHash type, const std::string& name);
		// 해당 타입의 리소스들을 모두 제거한다.
		void ReleaseResources(StringHash type);
		// 모든 리소스를 제거한다.
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
		// 왜 static_cast로 변환했을까...?
		return dynamic_cast<T*>(GetResource(type, name));
	}

	template<class T>
	void ResourceCache::ReleaseResource(const std::string& name)
	{
		StringHash type = T::GetTypeStatic();
		ReleaseResource(type, name);
	}
}