#pragma once
#include "core/Object.h"

namespace Dive
{
	// ResourceManager를 통합한 버전인데 만들다 만 것 같다.
	class DvResource : public Object
	{
		DV_CLASS(DvResource, Object)

	public:
		DvResource() = default;
		virtual ~DvResource() = default;

		template<class T>
		static std::shared_ptr<T> Load(const std::string& name);

	protected:
		virtual void loadResource(const std::string& name) = 0;

	private:
		static std::unordered_map <size_t, std::unordered_map<std::string, std::shared_ptr<DvResource>>> s_Resources;
		static std::mutex s_Mutex;
	};

	// 유니티의 경우 name은 '타입폴더/파일명'이다. 이때 확장자같은 건 없다.
	// 확장자를 포함해도 로드가 불가능하다고 한다.
	// => 이러한 구현을 실현하려면 엔진 포멧으로 변환이 필요하다.
	template<class T>
	std::shared_ptr<T> DvResource::Load(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		auto typeIt = s_Resources.find(T::GetTypeHashStatic());
		if (typeIt != s_Resources.end())
		{
			auto nameIt = typeIt->second.find(name);
			if (nameIt != typeIt->second.end())
				return std::dynamic_pointer_cast<T>(nameIt->second);
		}
		
		auto pNewResource = std::make_shared<T>();
		pNewResource->loadResource(name);
		s_Resources[T::GetTypeHashStatic()][name] = pNewResource;
		return pNewResource;
	}

	class Resource : public Object
	{
		DV_CLASS(Resource, Object)

	public:
		Resource() = default;
		virtual ~Resource() = default;

		const std::filesystem::path& GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::filesystem::path& path);

		virtual bool LoadFromFile(const std::filesystem::path& path) { return true; }
		virtual bool SaveToFile(const std::filesystem::path& path) { return true; }

	private:
		std::filesystem::path m_Filepath;
	};
}