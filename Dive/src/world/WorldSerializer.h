#pragma once

namespace Dive
{
	class World;

	class WorldSerializer
	{
	public:
		WorldSerializer(World* world);
		~WorldSerializer() = default;

		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		World* m_World;
	};
}
