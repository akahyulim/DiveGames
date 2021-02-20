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
		// get by path: 해당 폴더에서 T 타입 하나만?

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
			CORE_ERROR("잘못된 경로의 파일을 전달받았습니다. - {:s}", filepath);
			return std::shared_ptr<T>();
		}

		if (!File::IsEngineFile(filepath))
		{
			CORE_ERROR("Dive Engine에서 지원하는 형식의 파일이 아닙니다. - {:s}", filepath);
			return std::shared_ptr<T>();
		}

		auto resource = std::make_shared<T>(m_context);
		//resource->LoadFromFile(filepath);

		return Cache(std::dynamic_pointer_cast<Resource>(resource));
	}
}