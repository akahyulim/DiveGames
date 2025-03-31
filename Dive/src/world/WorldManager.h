#pragma once

namespace Dive
{
	class World;

	// 유니티에선 SceneLoaded, SceneUnloaded, ActiveSceneChanged 세 개의 이벤트도 다룬다.
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