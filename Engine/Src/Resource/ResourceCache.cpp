#include "DivePch.h"
#include "ResourceCache.h"
#include "Importer/ModelImporter.h"

namespace Dive
{
	static const uint64_t FIRST_ID = 0x1;
	static const uint64_t LAST_ID = 0xffffffffffffffff;

	uint64_t ResourceCache::m_CurResourceID = FIRST_ID;
	std::unordered_map<uint64_t, Resource*> ResourceCache::m_Resources;

	static ModelImporter* s_pModelImporter = nullptr;

	bool ResourceCache::Initialize()
	{
		s_pModelImporter = new ModelImporter;
		DV_ASSERT(s_pModelImporter);

		DV_CORE_TRACE("ResourceCache 초기화에 성공하였습니다.");

		return true;
	}

	void ResourceCache::Shutdown()
	{
		for (auto& resource : m_Resources)
			DV_DELETE(resource.second);
		m_Resources.clear();

		DV_DELETE(s_pModelImporter);

		DV_CORE_TRACE("ResoruceCache 종료에 성공하였습니다.");
	}

	void ResourceCache::RemoveByID(uint64_t id)
	{
		auto it = m_Resources.find(id);
		if (it != m_Resources.end())
			DV_DELETE(it->second);
		m_Resources.erase(it);

		DV_CORE_TRACE("Resource ID({0:d}) 객체를 제거하였습니다.", id);
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

	ModelImporter* ResourceCache::GetModelImporter()
	{
		return s_pModelImporter;
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