#pragma once

namespace Dive
{
	class Scene;

	class SceneSerializer
	{
	public:
		SceneSerializer(Scene* pScene);
		~SceneSerializer() = default;

		void Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

	private:
		Scene* m_pScene;
	};
}