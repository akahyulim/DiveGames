#pragma once

namespace Dive
{
	class World;

	class DvWorldSerializer
	{
	public:
		DvWorldSerializer(World* world);
		~DvWorldSerializer() = default;

		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		World* m_World;
	};
}