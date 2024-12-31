#pragma once

namespace Dive
{
	class World;

	class WorldSerializer
	{
	public:
		WorldSerializer(std::shared_ptr<World> pWorld);
		~WorldSerializer() = default;

		void Serialize(const std::filesystem::path& path);

		bool Deserialize(const std::filesystem::path& path);

	private:
		std::shared_ptr<World> m_pWorld;
	};
}