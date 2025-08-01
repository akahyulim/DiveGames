#pragma once
#include "core/Object.h"

namespace Dive
{
	enum class eResourceType
	{
		Undefined,
		Texture2D,
		RenderTexture,
		Cubemap,
		StaticMesh,
		SkinnedMesh,
		Material
	};

	class Resource : public Object
	{
	public:
		Resource(const std::string& name = "Resource");
		virtual ~Resource();

		const std::filesystem::path& GetFilepath() const { return m_filepath; }
		void SetFilepath(const std::filesystem::path& path);

		virtual bool LoadFromFile(const std::filesystem::path& path) { return true; }
		virtual bool SaveToFile(const std::filesystem::path& path) { return true; }

		static constexpr eResourceType GetType() { return eResourceType::Undefined; }

	protected:
		std::filesystem::path m_filepath{};
	};
}
