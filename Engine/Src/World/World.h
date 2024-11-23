#pragma once

namespace Dive
{
	class World
	{
	public:

		std::string GameName() const { return m_Name; }
		void Rename(const std::string& name) { m_Name = name; }

		static World* CreateWorld();
		static void DestroyWorld(World* pWorld);

	private:
		World();
		~World();

	private:
		std::string m_Name;

		static uint32_t s_WorldCounter;
	};
}