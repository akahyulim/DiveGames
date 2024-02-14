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
		// AssetImporter �߰�?

		DV_CORE_INFO("ResourceCache �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void ResourceCache::Shutdown()
	{
		for (auto& resource : m_Resources)
		{
			uint64_t id = resource.second->GetID();
			std::string name = resource.second->GetName();

			DV_DELETE(resource.second);

			DV_CORE_INFO("ResoruceCache���� ({0:s} - {1:d})�� �����Ͽ����ϴ�.", name, id);
		}
		m_Resources.clear();

		DV_CORE_INFO("ResoruceCache �����Ͽ����ϴ�.");
	}

	void ResourceCache::RemoveByID(uint64_t id)
	{
		auto it = m_Resources.find(id);
		if (it != m_Resources.end())
			DV_DELETE(it->second);
		m_Resources.erase(it);

		DV_CORE_INFO("Resource ID({0:d}) ��ü�� �����Ͽ����ϴ�.", id);
	}

	bool ResourceCache::IsCachedByID(uint64_t id)
	{
		if (m_Resources[id])
		{
			DV_CORE_WARN("�̹� ĳ�õ� ���ҽ�({0:s} : ({1:d})�Դϴ�.", m_Resources[id]->GetName(), id);
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