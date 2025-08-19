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
		void SetFilepath(const std::filesystem::path& path) { m_filepath = path; }

		virtual bool LoadFromFile(const std::filesystem::path& path) { return true; }
		virtual bool SaveToFile(const std::filesystem::path& path) { return true; }

		static constexpr eResourceType GetType() { return eResourceType::Undefined; }

		std::string GetName() const override;
		void SetName(const std::string& name) override;

	protected:
		std::filesystem::path m_filepath{};
	};
}
