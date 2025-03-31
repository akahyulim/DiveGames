
#pragma once

namespace Dive
{
	class World;

	class WorldSerializer
	{
	public:
		WorldSerializer(World* world);
		~WorldSerializer() = default;

		void Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

	private:
		World* m_World;
	};
}