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

		static void Tick(std::vector<TransformComponent*>& transformList);

		static void SetPosition(Entity entity, const DirectX::XMFLOAT3& pos);
		static void SetPosition(Entity entity, float x, float y, float z);

		static void SetLocalPosition(Entity entity, const DirectX::XMFLOAT3& pos);
		static void SetLocalPosition(Entity entity, float x, float y, float z);

		static DirectX::XMFLOAT3 GetPosition(Entity entity);
		static DirectX::XMFLOAT3 GetLocalPosition(Entity entity);

		static void SetRotationQuaternion(Entity entity, const DirectX::XMFLOAT4& rot);
		static void SetRotationByDegrees(Entity entity, const DirectX::XMFLOAT3& degrees);
		static void SetRotationByDegrees(Entity entity, float x, float y, float z);

		static void SetLocalRotationQuaternion(Entity entity, const DirectX::XMFLOAT4& rot);
		static void SetLocalRotationByDegrees(Entity entity, const DirectX::XMFLOAT3& degrees);
		static void SetLocalRotationByDegrees(Entity entity, float x, float y, float z);

		static DirectX::XMFLOAT4 GetRotationQuaternion(Entity entity);
		static DirectX::XMFLOAT4 GetLocalRotationQuaternion(Entity entity);

		static DirectX::XMFLOAT3 GetRotationDegrees(Entity entity);
		static DirectX::XMFLOAT3 GetLocalRotationDegrees(Entity entity);

		static void SetScale(Entity entity, const DirectX::XMFLOAT3& scl);
		static void SetScale(Entity entity, float x, float y, float z);

		static void Translate(Entity entity, const DirectX::XMFLOAT3& move, eSpace space = eSpace::World);
		static void Translate(Entity entity, float x, float y, float z, eSpace space = eSpace::World);

		static void Rotate(Entity entity, const DirectX::XMFLOAT3& degrees, eSpace space = eSpace::World);
		static void Rotate(Entity entity, float x, float y, float z, eSpace space = eSpace::World);

		static DirectX::XMFLOAT4X4 GetTransform(Entity entity);
		static DirectX::XMFLOAT4X4 GetLocalTransform(Entity entity);

		static void SetParent(Entity child, Entity parent);
		static Entity GetParent(Entity entity);
		static void RemoveParent(Entity child);

		static std::vector<Entity> GetChildren(Entity parent);
		static bool IsChildOf(Entity parent, Entity other);

		static std::vector<Entity> GetRootNodes(std::vector<Entity> gameObjecs);

		static bool HasParent(Entity entity);
		static bool HasChildren(Entity entity);

	private:
		static std::unordered_map<entt::entity, HierarchyNode> s_Hierarchy;
		
	};
}