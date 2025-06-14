#pragma once

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

	class Resource
	{
	public:
		Resource(const std::string& name = "Resource");
		Resource(UINT64 instanceID, const std::string& name = "Resource");
		virtual ~Resource();

		UINT64 GetInstanceID() const { return m_InstanceID; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		const std::filesystem::path& GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::filesystem::path& path);

		virtual bool LoadFromFile(const std::filesystem::path& path) { return true; }
		virtual bool SaveToFile(const std::filesystem::path& path) { return true; }

		static constexpr eResourceType GetType() { return eResourceType::Undefined; }

	protected:
		UINT64 m_InstanceID;
		std::string m_Name;

		std::filesystem::path m_Filepath{};
	};
}
