#pragma once
#include "core/Object.h"

namespace Dive
{
	// ResourceManager�� ������ �����ε� ����� �� �� ����.
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

	// ����Ƽ�� ��� name�� 'Ÿ������/���ϸ�'�̴�. �̶� Ȯ���ڰ��� �� ����.
	// Ȯ���ڸ� �����ص� �ε尡 �Ұ����ϴٰ� �Ѵ�.
	// => �̷��� ������ �����Ϸ��� ���� �������� ��ȯ�� �ʿ��ϴ�.
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