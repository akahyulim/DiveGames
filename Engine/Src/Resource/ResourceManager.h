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
		ResourceManager(const ResourceManager&) = delete;
		void operator=(const ResourceManager&) = delete;

		static ResourceManager* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new ResourceManager;

			return s_pInstance;
		}

		bool AddManualResource(Resource* pResource);
		template<class T> T* GetResource(const std::string& name);

		void Shutdown();

	private:
		ResourceManager();
		~ResourceManager();

		Resource* findResource(size_t typeHash, size_t nameHash);

	private:
		static ResourceManager* s_pInstance;

		std::unordered_map<size_t, ResourceGroup> m_ResourceGroups;
	};

	template<class T>
	T* ResourceManager::GetResource(const std::string& name)
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
		m_ResourceGroups[typeHash].emplace(nameHash, static_cast<Resource*>(pNewObject));

		return pNewObject;
	}

	ResourceManager* GetResourceManager();
}