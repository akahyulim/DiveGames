#pragma once

namespace Dive
{
	class World;

	// ����Ƽ���� SceneLoaded, SceneUnloaded, ActiveSceneChanged �� ���� �̺�Ʈ�� �ٷ��.
	class WorldManager
	{
	public:
		static World* CreateWorld(const std::string& name = "NewWorld");

		static World* LoadFromFile(const std::filesystem::path& path);
		static void SaveToFile(const std::filesystem::path& path = "");

		static void UnloadWorld();

		static World* GetActiveWorld() { return s_ActiveWorld.get(); }

	private:
		static std::unique_ptr<World> s_ActiveWorld;
	};
}