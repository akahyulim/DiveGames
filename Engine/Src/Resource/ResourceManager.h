#pragma once
#include "Core/Object.h"
#include "Core/Log.h"
#include "Core/FileSystemHelper.h"
#include "Resource.h"

namespace Dive
{

	class ResourceManager : public Object
	{
		DIVE_OBJECT(ResourceManager, Object);

	public:
		ResourceManager(Context* context);
		~ResourceManager();

		template<class T>
		std::shared_ptr<T> Load(const std::string& filepath);

		std::shared_ptr<Resource> Cache(const std::shared_ptr<Resource>& resource);
		bool IsCached(const eResourceType type, const std::string& name);
		void Remove(std::shared_ptr<Resource> resource);

		std::shared_ptr<Resource> GetByName(const eResourceType type, const std::string& name);
		std::vector<std::shared_ptr<Resource>> GetByType(eResourceType type);
		// get by path: �ش� �������� T Ÿ�� �ϳ���?

		unsigned int GetResourceCount(eResourceType type);

	private:
	private:
		std::vector<std::shared_ptr<Resource>> m_resources;
	};

	template<class T>
	inline std::shared_ptr<T> ResourceManager::Load(const std::string & filepath)
	{
		if (!File::IsExistFile(filepath))
		{
			CORE_ERROR("�߸��� ����� ������ ���޹޾ҽ��ϴ�. - {:s}", filepath);
			return std::shared_ptr<T>();
		}

		if (!File::IsEngineFile(filepath))
		{
			CORE_ERROR("Dive Engine���� �����ϴ� ������ ������ �ƴմϴ�. - {:s}", filepath);
			return std::shared_ptr<T>();
		}

		auto resource = std::make_shared<T>(m_context);
		//resource->LoadFromFile(filepath);

		return Cache(std::dynamic_pointer_cast<Resource>(resource));
	}
}