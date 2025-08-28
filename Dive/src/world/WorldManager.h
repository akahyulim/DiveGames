#pragma once

namespace Dive
{
	class World;

	//https://docs.unity3d.com/ScriptReference/SceneManagement.SceneManager.html
	class WorldManager
	{
	public:
		static bool Initialize();
		static void Clear();

		static World* CreateWorld(const std::string& name);

		static World* LoadWorld(const std::filesystem::path& filepath);
		static void UnloadWorld();

		static void OnUpdate(EventData data);

		static World* GetActiveWorld();

	private:
		static World* s_activeWorld;	// 일단 단일 World로 관리
	};
}