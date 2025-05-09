#pragma once
#include "core/UUID.h"
#include "world/Entity.h"
#include <yaml-cpp/yaml.h>

namespace Dive
{
	enum class eSpace
	{
		Self,
		World
	};

	class Transforms
	{
	public:
		struct HierarchyNode
		{
			Entity Parent{};
			std::vector<Entity> Children;
		};

		static void Initialize();
		static void Shutdown();

		static void Tick(std::vector<Entity>& entities);

		// Transform
		static DirectX::XMFLOAT3 GetWorldPosition(Entity& entity);
		static void SetWorldPosition(Entity& entity, const DirectX::XMFLOAT3& pos);
		static void SetLocalPosition(Entity& entity, const DirectX::XMFLOAT3& pos);

		static DirectX::XMFLOAT4 GetWorldRotationQuaternion(Entity& entity);
		static void SetWorldRotationQuaternion(Entity& entity, const DirectX::XMFLOAT4& rot);
		static void SetLocalRotationQuaternion(Entity& entity, const DirectX::XMFLOAT4& rot);

		static DirectX::XMFLOAT3 GetWorldRotationDegrees(Entity& entity);
		static void SetWorldRotationDegrees(Entity& entity, const DirectX::XMFLOAT3& rot);
		static DirectX::XMFLOAT3 GetLocalRotationDegrees(Entity& entity);
		static void SetLocaldRotationDegrees(Entity& entity, const DirectX::XMFLOAT3& rot);

		static DirectX::XMFLOAT3 GetWorldRotationRadians(Entity& entity);
		static void SetWorldRotationRadians(Entity& entity, const DirectX::XMFLOAT3& rot);
		static DirectX::XMFLOAT3 GetLocalRotationRadians(Entity& entity);
		static void SetLocalRotationRadians(Entity& entity, const DirectX::XMFLOAT3& rot);

		static DirectX::XMFLOAT3 GetWorldScale(Entity& entity);
		static void SetWorldScale(Entity& entity, const DirectX::XMFLOAT3& scale);
		static void SetLocalScale(Entity& entity, const DirectX::XMFLOAT3& scale);

		static void Translate(Entity& entity, const DirectX::XMFLOAT3& move, eSpace space = eSpace::World);

		static void RotateQuaternion(Entity& entity, const DirectX::XMFLOAT4& quat, eSpace space = eSpace::World);
		static void RotateDegrees(Entity& entity, const DirectX::XMFLOAT3& degrees, eSpace space = eSpace::World);
		static void RotateRadians(Entity& entity, const DirectX::XMFLOAT3& radians, eSpace space = eSpace::World);

		static DirectX::XMFLOAT4X4 GetLocalMatrix(Entity& entity);

		// Hierarchy
		static void SetParent(Entity child, Entity parent);
		static Entity GetParent(Entity entity);
		static void RemoveParent(Entity child);

		static std::vector<Entity> GetChildren(Entity parent);
		static bool IsChildOf(Entity parent, Entity other);

		static std::vector<Entity> GetRootNodes(std::vector<Entity> gameObjecs);

		static bool HasParent(Entity entity);
		static bool HasChildren(Entity entity);

	private:
		static DirectX::XMFLOAT4X4 calcWorldMatrix(Entity& entity);
		static void update(Entity& entity);

	private:
		static std::unordered_map<entt::entity, HierarchyNode> s_Hierarchy;
		
	};
}