#pragma once
#include "World.h"

namespace Dive
{
	class WorldSerializer
	{
	public:
		WorldSerializer(const std::shared_ptr<World> world);
		~WorldSerializer() = default;

		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		std::shared_ptr<World> m_World;
	};
}