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
		bool Deserialize(ID3D11Device* device, const std::filesystem::path& filepath);

	private:
		World* m_world;
	};
}
