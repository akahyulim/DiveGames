#pragma once
#include "SystemBase.h"

namespace Dive
{
	class World;

	class ParentSystem : public SystemBase
	{
	public:
		ParentSystem(World* world) : SystemBase(world) {}
		~ParentSystem() = default;

		void OnUpdate() override;
	};

	class LocalToWorldSystem : public SystemBase
	{
	public:
		LocalToWorldSystem(World* world) : SystemBase(world) {}
		~LocalToWorldSystem() = default;

		void OnUpdate() override;
	};
}
