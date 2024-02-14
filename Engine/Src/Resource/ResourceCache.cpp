#include "DivePch.h"
#include "ResourceCache.h"

namespace Dive
{
	static constexpr uint64_t FIRST_ID = 0x1;
	static constexpr uint64_t LAST_ID = 0xffffffffffffffff;

	uint64_t ResourceCache::m_CurResourceID = FIRST_ID;
	std::unordered_map<uint64_t, Resource*> ResourceCache::m_Resources;

	bool ResourceCache::Initialize()
	{
		// AssetImporter 추가?

		DV_CORE_INFO("ResourceCache 초기화에 성공하였습니다.");

		return true;
	}

	void ResourceCache::Shutdown()
	{
		for (auto& resource : m_Resources)
		{
			uint64_t id = resource.second->GetID();
			std::string name = resource.second->GetName();

			DV_DELETE(resource.second);

			DV_CORE_INFO("ResoruceCache에서 ({0:s} - {1:d})를 제거하였습니다.", name, id);
		}
		m_Resources.clear();

		DV_CORE_INFO("ResoruceCache 종료하였습니다.");
	}

	void ResourceCache::RemoveByID(uint64_t id)
	{
		auto it = m_Resources.find(id);
		if (it != m_Resources.end())
			DV_DELETE(it->second);
		m_Resources.erase(it);

		DV_CORE_INFO("Resource ID({0:d}) 객체를 제거하였습니다.", id);
	}

	bool ResourceCache::IsCachedByID(uint64_t id)
	{
		if (m_Resources[id])
		{
			DV_CORE_WARN("이미 캐시된 리소스({0:s} : ({1:d})입니다.", m_Resources[id]->GetName(), id);
			return true;
		}

		return false;
	}

	uint64_t ResourceCache::getFreeID()
	{
		auto checkID = m_CurResourceID;

		for (;;)
		{
			auto freeID = m_CurResourceID;

			if (m_CurResourceID < LAST_ID)
				++m_CurResourceID;
			else
				m_CurResourceID = FIRST_ID;

			if (checkID == m_CurResourceID)
				return 0;
			else if (!m_Resources[freeID])
				return freeID;
		}
	}
}