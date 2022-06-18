#pragma once
#include "Base/Base.h"
#include "Resource.h"
#include "Helper/FileSystem.h"
#include "Renderer/Graphics/Texture2D.h"
#include "Renderer/Material.h"

namespace Dive
{
	class ResourceManager
	{
	public:
		static ResourceManager& GetInstance()
		{
			static ResourceManager inst;
			return inst;
		}

		void Clear();

		template<class T>
		T* Cache(T* pResource)
		{
			if (pResource == nullptr)
				return nullptr;

			auto name = pResource->GetName();
			if (HasResource<T>(name))
				return GetResource<T>(name);

			// �� ������ save to file�� �� �� �Ѵ�.
			// �̶����� resource�� engine file path�� ������ �Ѵ�.
			// �׷��� ����Ǿ���, �ֽ� ���� ������ SaveToDataFile()���� �ٽ� ���鶧 �����Ѵ�.

			return dynamic_cast<T*>(m_Resources.emplace_back(pResource));
		}

		// bug
		// �̹� ������ ���ҽ��� ���� ��� �ش� ��ü�� �����Ѵ�.
		// �ؽ����� ��� �׳� ����ص� ������ ������
		// ���� ��� ���ο� ���� ������Ʈ�� �������� ���ϰ� �ִ�.
		// ���ĸ�ź �ڵ带 ������ ���ε�
		// �� �Լ��� ������⺸�� ���� �߰��ϴ� �гο��� �����ؾ� �� �� ����.
		template<class T>
		T* Load(const std::string& filepath)
		{
			if (!std::filesystem::exists(filepath))
			{
				DV_CORE_WARN("�߸��� ��θ� ���޹޾ҽ��ϴ� - {:s}", filepath);
				return nullptr;
			}

			auto name = Helper::FileSystem::GetFileNameWithoutExtension(filepath);

			if (HasResource<T>(name))
				return GetResource<T>(name);

			auto pCreatedResource = new T();
			pCreatedResource->SetName(name);
			if (!pCreatedResource->LoadFromFile(filepath))
			{
				DV_CORE_ERROR("���� �ε忡 �����Ͽ����ϴ� - {:s}", filepath);
				return nullptr;
			}

			return Cache<T>(pCreatedResource);
		}

		bool HasResource(const std::string& name, const eResourceType type);
		template<class T>
		bool HasResource(const std::string& name)
		{
			return HasResource(name, T::GetStaticType());
		}
		bool HasResourceByID(unsigned long long id);

		template<class T>
		void Remove(Resource* pResource)
		{
			if (pResource == nullptr)
				return;

			if (!HasResourceByID(pResource->GetInstanceID()))
				return;

			for (auto it = m_Resources.begin(); it != m_Resources.end(); it++)
			{
				if ((*it)->GetInstanceID() == pResource->GetInstanceID())
				{
					DV_DELETE(*it);
					m_Resources.erase(it);
					return;
				}
			}
		}
		
		Resource* GetResource(const std::string& name, const eResourceType type);
		template<class T>
		T* GetResource(const std::string& name)
		{
			return dynamic_cast<T*>(GetResource(name, T::GetStaticType()));
		}
		Resource* GetResourceByID(unsigned long long id);

		unsigned int GetResourceCount() const { return static_cast<unsigned int>(m_Resources.size()); }

		bool SaveToDataFile();
		bool LoadFromDataFile();

	private:
		std::vector<Resource*> m_Resources;
	};
}