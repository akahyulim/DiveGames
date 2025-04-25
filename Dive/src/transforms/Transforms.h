#pragma once
#include "core/UUID.h"
#include "world/GameObject.h"
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
			GameObject Parent = {};
			std::vector<GameObject> Children;
		};

		static void Initialize();
		static void Shutdown();

		static void Tick(std::vector<TransformComponent*>& transformList);

		static void SetPosition(GameObject gameObject, const DirectX::XMFLOAT3& pos);
		static void SetPosition(GameObject gameObject, float x, float y, float z);

		static void SetLocalPosition(GameObject gameObject, const DirectX::XMFLOAT3& pos);
		static void SetLocalPosition(GameObject gameObject, float x, float y, float z);

		static DirectX::XMFLOAT3 GetPosition(GameObject gameObject);
		static DirectX::XMFLOAT3 GetLocalPosition(GameObject gameObject);

		static void SetRotationQuaternion(GameObject gameObject, const DirectX::XMFLOAT4& rot);
		static void SetRotationByDegrees(GameObject gameObject, const DirectX::XMFLOAT3& degrees);
		static void SetRotationByDegrees(GameObject gameObject, float x, float y, float z);

		static void SetLocalRotationQuaternion(GameObject gameObject, const DirectX::XMFLOAT4& rot);
		static void SetLocalRotationByDegrees(GameObject gameObject, const DirectX::XMFLOAT3& degrees);
		static void SetLocalRotationByDegrees(GameObject gameObject, float x, float y, float z);

		static DirectX::XMFLOAT4 GetRotationQuaternion(GameObject gameObject);
		static DirectX::XMFLOAT4 GetLocalRotationQuaternion(GameObject gameObject);

		static DirectX::XMFLOAT3 GetRotationDegrees(GameObject gameObject);
		static DirectX::XMFLOAT3 GetLocalRotationDegrees(GameObject gameObject);

		static void SetScale(GameObject gameObject, const DirectX::XMFLOAT3& scl);
		static void SetScale(GameObject gameObject, float x, float y, float z);

		static void Translate(GameObject gameObject, const DirectX::XMFLOAT3& move, eSpace space = eSpace::World);
		static void Translate(GameObject gameObject, float x, float y, float z, eSpace space = eSpace::World);

		static void Rotate(GameObject gameObject, const DirectX::XMFLOAT3& degrees, eSpace space = eSpace::World);
		static void Rotate(GameObject gameObject, float x, float y, float z, eSpace space = eSpace::World);

		static DirectX::XMFLOAT4X4 GetTransform(GameObject gameObject);
		static DirectX::XMFLOAT4X4 GetLocalTransform(GameObject gameObject);

		static void SetParent(GameObject child, GameObject parent);
		static GameObject GetParent(GameObject gameObject);
		static void RemoveParent(GameObject child);

		static std::vector<GameObject> GetChildren(GameObject parent);
		static bool IsChildOf(GameObject parent, GameObject other);

		static std::vector<GameObject> GetRootNodes(std::vector<GameObject> gameObjecs);

		static bool HasParent(GameObject gameObject);
		static bool HasChildren(GameObject gameObject);

	private:
		static std::unordered_map<entt::entity, HierarchyNode> s_Hierarchy;
		
	};
}