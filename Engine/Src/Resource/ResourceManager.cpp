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

	// Cache된 Resource들을 각각 파일화한 후
	// 이들을 묶은 SceneName.dat 파일로 만든다.
	// => 역시 빌드 결과 파일 구조가 걸린다.
	// .dat가 리소스만 가진다는 게 불합리해 보인다.
	// 일단은 최대한 유니티의 Asset 구조를 따라가자.
	bool ResourceManager::SaveToDataFile()
	{
		// 임시: Asset의 어느 폴더에 넣어야 할까?
		auto filepath = "test_World.dat";

		auto fileStream = FileStream(filepath, eFileStreamMode::Write);
		if (!fileStream.IsOpen())
			return false;

		auto count = m_Resources.size();
		fileStream.Write(count);

		for (auto pResource : m_Resources)
		{
			// 이 곳에서 파일 이름을 먼저 정하는 것이 나을 것 같다.
			// 아니면 역순으로 Resource를 파일화한 후 이름을 얻을 수 있다.
			fileStream.Write(pResource->GetFilepath());
			fileStream.Write(static_cast<uint32_t>(pResource->GetType()));

			// 그것도 아니면 fileStream을 넘겨 하나의 파일로 만들 수 있다...
			// 물론 그 경우 Load가 힘들어진다...
			pResource->SaveToFile(pResource->GetFilepath());
		}

		fileStream.Close();

		return true;
	}
	
	// SceneName.dat 파일로부터 Resource를 생성하여 Cache한다.
	// 유니티는 이외에도 Asset 폴더에 외부 파일을 넣으면 자동으로 임포트한다.
	bool ResourceManager::LoadFromDataFile()
	{
		// 임시: Asset의 어느 폴더에 넣어야 할까?
		auto filepath = "test_World.dat";

		auto fileStream = FileStream(filepath, eFileStreamMode::Read);
		if (!fileStream.IsOpen())
			return false;

		const auto count = fileStream.ReadAs<uint32_t>();

		for (uint32_t i = 0; i < count; i++)
		{
			const auto filePath = fileStream.ReadAs<std::string>();
			const auto type = static_cast<eResourceType>(fileStream.ReadAs<uint32_t>());

			switch (type)
			{
			case eResourceType::Model:
				Load<Model>(filepath);
			}
		}

		fileStream.Close();

		return true;
	}
}