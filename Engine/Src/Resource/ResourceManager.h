#pragma once
#include "Resource.h"
#include "Core/CoreDefs.h"
#include "IO/FileSystem.h"
#include "Core/Log.h"

namespace Dive
{
	using ResourceGroup = std::unordered_map<size_t, Resource*>;

	class ResourceManager
	{
	public:
	
	public:
		static bool AddManualResource(Resource* pResource);
		template<class T> static T* GetResource(const std::string& name);

		static void Shutdown();

	private:
		static Resource* findResource(size_t typeHash, size_t nameHash);

	private:
		static std::unordered_map<size_t, ResourceGroup> s_ResourceGroups;
	};

	template<class T>
	static T* ResourceManager::GetResource(const std::string& name)
	{
		// 추가적인 가공이 필요할 수 있다.
		size_t nameHash = std::hash<std::string>{}(name);
		size_t typeHash = T::GetTypeHashStatic();

		Resource* pExistedObject = findResource(typeHash, nameHash);
		if (pExistedObject)
			return dynamic_cast<T*>(pExistedObject);

		T* pNewObject = new T;
		if (!pNewObject->LoadFromFile(name))
		{
			DV_DELETE(pNewObject);
			return nullptr;
		}

		pNewObject->SetName(name);
		s_ResourceGroups[typeHash].emplace(nameHash, static_cast<Resource*>(pNewObject));

		return pNewObject;
	}
}