#pragma once
#include "core/Object.h"

namespace Dive
{
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
