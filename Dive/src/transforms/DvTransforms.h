#pragma once
#include "world/Components.h"
#include <entt/entt.hpp>

#include "Transforms.h"

namespace Dive
{
	class DvTransforms
	{
	public:
		static void Initailize();
		static void Shutdown();

		static void Update(std::vector<TransformComponent*> transformComponents);

		// Transform
		static DirectX::XMFLOAT3 GetWorldPosition(const TransformComponent& tc);
		static void SetWorldPosition(TransformComponent& tc, const DirectX::XMFLOAT3& pos);
		static void SetLocalPosition(TransformComponent& tc, const DirectX::XMFLOAT3& pos);

		static DirectX::XMFLOAT4 GetWorldRotationQuaternion(const TransformComponent& tc);
		static void SetWorldRotationQuaternion(TransformComponent& tc, const DirectX::XMFLOAT4& rot);
		static void SetLocalRotationQuaternion(TransformComponent& tc, const DirectX::XMFLOAT4& rot);

		static DirectX::XMFLOAT3 GetWorldRotationDegrees(const TransformComponent& tc);
		static void SetWorldRotationDegrees(TransformComponent& tc, const DirectX::XMFLOAT3& rot);
		static DirectX::XMFLOAT3 GetLocalRotationDegrees(const TransformComponent& tc);
		static void SetLocaldRotationDegrees(TransformComponent& tc, const DirectX::XMFLOAT3& rot);

		static DirectX::XMFLOAT3 GetWorldRotationRadians(const TransformComponent& tc);
		static void SetWorldRotationRadians(TransformComponent& tc, const DirectX::XMFLOAT3& rot);
		static DirectX::XMFLOAT3 GetLocalRotationRadians(const TransformComponent& tc);
		static void SetLocalRotationRadians(TransformComponent& tc, const DirectX::XMFLOAT3& rot);

		static DirectX::XMFLOAT3 GetWorldScale(const TransformComponent& tc);
		static void SetWorldScale(TransformComponent& tc, const DirectX::XMFLOAT3& scale);
		static void SetLocalScale(TransformComponent& tc, const DirectX::XMFLOAT3& scale);

		static void Translate(TransformComponent& tc, const DirectX::XMFLOAT3& move, eSpace space = eSpace::World);

		static void RotateQuaternion(TransformComponent& tc, const DirectX::XMFLOAT4& quat, eSpace space = eSpace::World);
		static void RotateDegrees(TransformComponent& tc, const DirectX::XMFLOAT3& degrees, eSpace space = eSpace::World);
		static void RotateRadians(TransformComponent& tc, const DirectX::XMFLOAT3& radians, eSpace space = eSpace::World);

		static DirectX::XMFLOAT4X4 GetLocalMatrix(const TransformComponent& tc);

		// Hierarchy
		static Entity* GetParent(Entity& entity);
		static void SetParent(Entity child, Entity parent);

		static bool IsChildOf(Entity& parent, Entity& child);

		static std::vector<Entity*>& GetChildren(Entity& entity);

		static bool HasParent(Entity& entity);
		static bool HasChild(Entity& entity);

		static std::vector<Entity> GetRootNodes(std::vector<Entity> gameObjects);
		
		//static TransformComponent* GetParent(const TransformComponent& tc);
		//static void SetParent(TransformComponent& tc, TransformComponent* parent);

		//static bool IsChildOf(const TransformComponent& parent, const TransformComponent& child);

		//static std::vector<Entity*>& GetChildren(TransformComponent& tc);

		//static bool HasParent(const TransformComponent& tc);
		//static bool HasChild(const TransformComponent& tc);


	private:
		static DirectX::XMFLOAT4X4 calcWorldMatrix(const TransformComponent& tc);
	};
}
