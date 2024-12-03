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

		const std::string& GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::string& filepath);

		virtual bool LoadFromFile(const std::string& filename) { return true; }
		virtual bool SaveToFile(const std::string& filename) { return true; }

	private:
		std::string m_Filepath;
	};
}