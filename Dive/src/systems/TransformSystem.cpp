#include "stdafx.h"
#include "TransformSystem.h"
#include "world/EntityManager.h"
#include "world/DvWorld.h"

namespace Dive
{
	TransformSystem::TransformSystem(DvWorld* world)
		: SystemBase(world)
	{
	}

	void TransformSystem::OnUpdate()
	{
		auto& entityManager = m_World->GetEntityManager();
		auto view = entityManager.GetEntityView<DvTransformComponent>();

		for (auto [entity, tc]  : view.each())
		{
			auto localMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&tc.Scale))
				* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&tc.Rotation))
				* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tc.Position));

			DirectX::XMStoreFloat4x4(&tc.LocalToWorld, localMatrix);
		}
	}
}