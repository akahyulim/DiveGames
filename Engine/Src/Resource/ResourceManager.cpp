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

	// 일단 외부 파일의 경로만 저장한다.
	bool ResourceManager::SaveToDataFile()
	{
		// 임시: Asset의 어느 폴더에 넣어야 할까?
		auto filepath = "Assets/Scenes/test_World.dat";

		auto fileStream = FileStream(filepath, eFileStreamMode::Write);
		if (!fileStream.IsOpen())
			return false;

		auto count = static_cast<uint32_t>(m_Resources.size());
		fileStream.Write(count);

		for (auto pResource : m_Resources)
		{
			// 이 곳에서 Engine Extension 경로로 저장해야 한다.
			auto resourcePath = pResource->GetEngineFilepath();
			auto type = static_cast<uint32_t>(pResource->GetType());

			fileStream.Write(resourcePath);
			fileStream.Write(type);

			pResource->SaveToFile(resourcePath);
		}

		fileStream.Close();

		return true;
	}
	
	bool ResourceManager::LoadFromDataFile()
	{
		// 임시: Asset의 어느 폴더에 넣어야 할까?
		auto filepath = "Assets/Scenes/test_World.dat";

		auto fileStream = FileStream(filepath, eFileStreamMode::Read);
		if (!fileStream.IsOpen())
			return false;

		const auto count = fileStream.ReadAs<uint32_t>();

		for (uint32_t i = 0; i < count; i++)
		{
			const auto resourcePath = fileStream.ReadAs<std::string>();
			const auto type = static_cast<eResourceType>(fileStream.ReadAs<uint32_t>());

			switch (type)
			{
			case eResourceType::Texture2D:
				Load<Texture2D>(resourcePath);
				break;

			case eResourceType::Model:
				Load<Model>(resourcePath);
				break;
			}
		}

		fileStream.Close();

		return true;
	}
}