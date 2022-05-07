#include "divepch.h"
#include "ResourceManager.h"
#include "FileStream.h"
#include "Renderer/Model.h"

namespace Dive
{
	void ResourceManager::Clear()
	{
		if (m_Resources.empty())
			return;

		for (auto pResource : m_Resources)
		{
			DV_DELETE(pResource);
		}
		m_Resources.clear();
		m_Resources.shrink_to_fit();
	}

	bool ResourceManager::HasResource(const std::string& name, const eResourceType type)
	{
		if(!m_Resources.empty())
		{
			for (auto pResource : m_Resources)
			{
				if ((pResource->GetName() == name) && (pResource->GetType() == type))
					return true;
			}
		}

		return false;
	}

	bool ResourceManager::HasResourceByID(unsigned long long id)
	{
		if (!m_Resources.empty())
		{
			for (auto pResource : m_Resources)
			{
				if (pResource->GetInstanceID() == id)
					return true;
			}
		}

		return false;
	}

	Resource* ResourceManager::GetResource(const std::string& name, const eResourceType type)
	{
		if (!m_Resources.empty())
		{
			for (auto pResource : m_Resources)
			{
				if ((pResource->GetName() == name) && (pResource->GetType() == type))
					return pResource;
			}
		}

		return nullptr;
	}

	Resource* ResourceManager::GetResourceByID(unsigned long long id)
	{
		if (!m_Resources.empty())
		{
			for (auto pResource : m_Resources)
			{
				if (pResource->GetInstanceID() == id)
					return pResource;
			}
		}

		return nullptr;
	}

	// �ϴ� �ܺ� ������ ��θ� �����Ѵ�.
	bool ResourceManager::SaveToDataFile()
	{
		// �ӽ�: Asset�� ��� ������ �־�� �ұ�?
		auto filepath = "test_World.dat";

		auto fileStream = FileStream(filepath, eFileStreamMode::Write);
		if (!fileStream.IsOpen())
			return false;

		auto count = static_cast<uint32_t>(m_Resources.size());
		fileStream.Write(count);

		for (auto pResource : m_Resources)
		{
			auto file = pResource->GetFilepath();
			auto type = static_cast<uint32_t>(pResource->GetType());

			fileStream.Write(file);
			fileStream.Write(type);
		}

		fileStream.Close();

		return true;
	}
	
	bool ResourceManager::LoadFromDataFile()
	{
		// �ӽ�: Asset�� ��� ������ �־�� �ұ�?
		auto filepath = "test_World.dat";

		auto fileStream = FileStream(filepath, eFileStreamMode::Read);
		if (!fileStream.IsOpen())
			return false;

		const auto count = fileStream.ReadAs<uint32_t>();

		for (uint32_t i = 0; i < count; i++)
		{
			const auto file = fileStream.ReadAs<std::string>();
			const auto type = static_cast<eResourceType>(fileStream.ReadAs<uint32_t>());

			switch (type)
			{
			case eResourceType::Texture2D:
				Load<Texture2D>(file);
				break;
			}
		}

		fileStream.Close();

		return true;
	}
}