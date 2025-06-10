#pragma once
#include "SystemBase.h"

namespace Dive
{
	class World;

	// 리소스 버퍼 생성 및 갱신
	class RenderMeshSystem : public SystemBase
	{
	public:
		RenderMeshSystem(World* world) : SystemBase(world) {}
		~RenderMeshSystem() override = default;

		void OnUpdate();
	};

	// GPU에 바인딩 후 draw call
	class EntitiesGraphicsSystem : public SystemBase
	{
	public:
		EntitiesGraphicsSystem(World* world) : SystemBase(world) {}
		~EntitiesGraphicsSystem() override = default;

		void OnUpdate();
	};
}