#include "stdafx.h"
#include "transforms.h"
#include "core/CoreDefs.h"
#include "math/Math.h"

namespace Dive
{
	std::unordered_map<entt::entity, Transforms::HierarchyNode> Transforms::s_Hierarchy;

	void Transforms::Initialize()
	{
	}
	
	void Transforms::Shutdown()
	{
	}

	void Transforms::Tick()
	{
	}

	void Transforms::SetPosition(GameObject gameObject, const DirectX::XMFLOAT3& pos)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();
		tc.Position = pos;
	}

	void Transforms::SetPosition(GameObject gameObject, float x, float y, float z)
	{
		SetPosition(gameObject, { x, y, z });
	}

	void Transforms::SetLocalPosition(GameObject gameObject, const DirectX::XMFLOAT3& pos)
	{
		if (!HasParent(gameObject))
		{
			SetPosition(gameObject, pos);
			return;
		}

		auto parentTransform = GetTransform(GetParent(gameObject));
		auto parentTransformMat = DirectX::XMLoadFloat4x4(&parentTransform);

		auto localPositionVec = DirectX::XMLoadFloat3(&pos);

		auto worldPositionVec = DirectX::XMVector3TransformCoord(localPositionVec, parentTransformMat);

		DirectX::XMFLOAT3 worldPosition{};
		DirectX::XMStoreFloat3(&worldPosition, worldPositionVec);

		SetPosition(gameObject, worldPosition);
	}

	void Transforms::SetLocalPosition(GameObject gameObject, float x, float y, float z)
	{
		SetLocalPosition(gameObject, { x, y, z });
	}

	DirectX::XMFLOAT3 Transforms::GetPosition(GameObject gameObject)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();
		return tc.Position;
	}

	DirectX::XMFLOAT3 Transforms::GetLocalPosition(GameObject gameObject)
	{
		if (!HasParent(gameObject))
			return GetPosition(gameObject);

		auto parentTransform = GetTransform(GetParent(gameObject));
		auto parentTransformInverseMat = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&parentTransform));

		auto& tc = gameObject.GetComponent<TransformComponent>();
		auto positionVec = DirectX::XMLoadFloat3(&tc.Position);

		auto localPositionVec = DirectX::XMVector3TransformCoord(positionVec, parentTransformInverseMat);
		
		DirectX::XMFLOAT3 localPosition{};
		DirectX::XMStoreFloat3(&localPosition, localPositionVec);

		return localPosition;
	}

	void Transforms::SetRotationQuaternion(GameObject gameObject, const DirectX::XMFLOAT4& rot)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();
		tc.Rotation = rot;
	}

	void Transforms::SetRotationByDegrees(GameObject gameObject, const DirectX::XMFLOAT3& degrees)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();

		auto radiansX = DirectX::XMConvertToRadians(degrees.x);
		auto radiansY = DirectX::XMConvertToRadians(degrees.y);
		auto radiansZ = DirectX::XMConvertToRadians(degrees.z);

		auto rotationVec = DirectX::XMQuaternionRotationRollPitchYaw(radiansX, radiansY, radiansZ);
		rotationVec = DirectX::XMQuaternionNormalize(rotationVec);

		DirectX::XMStoreFloat4(&tc.Rotation, rotationVec);
	}

	void Transforms::SetRotationByDegrees(GameObject gameObject, float x, float y, float z)
	{
		SetRotationByDegrees(gameObject, { x, y, z });
	}

	void Transforms::SetLocalRotationQuaternion(GameObject gameObject, const DirectX::XMFLOAT4& rot)
	{
		if (!HasParent(gameObject))
		{
			SetRotationQuaternion(gameObject, rot);
			return;
		}

		auto parentRotation = GetParent(gameObject).GetComponent<TransformComponent>().Rotation;
		auto parentRotationVec = DirectX::XMLoadFloat4(&parentRotation);
		auto localRotationVec = DirectX::XMLoadFloat4(&rot);

		auto worldRotationVec = DirectX::XMQuaternionMultiply(parentRotationVec, localRotationVec);
		worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);

		DirectX::XMFLOAT4 worldRotation{};
		DirectX::XMStoreFloat4(&worldRotation, worldRotationVec);

		SetRotationQuaternion(gameObject, worldRotation);
	}

	void Transforms::SetLocalRotationByDegrees(GameObject gameObject, const DirectX::XMFLOAT3& degrees)
	{
		if (!HasParent(gameObject))
		{
			SetRotationByDegrees(gameObject, degrees);
			return;
		}

		auto parentRotation = GetParent(gameObject).GetComponent<TransformComponent>().Rotation;
		auto parentRotationVec = DirectX::XMLoadFloat4(&parentRotation);

		auto& tc = gameObject.GetComponent<TransformComponent>();

		auto radiansX = DirectX::XMConvertToRadians(degrees.x);
		auto radiansY = DirectX::XMConvertToRadians(degrees.y);
		auto radiansZ = DirectX::XMConvertToRadians(degrees.z);

		auto localRotationVec = DirectX::XMQuaternionRotationRollPitchYaw(radiansX, radiansY, radiansZ);

		auto worldRotationVec = DirectX::XMQuaternionMultiply(parentRotationVec, localRotationVec);
		worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);

		DirectX::XMFLOAT4 worldRotation{};
		DirectX::XMStoreFloat4(&worldRotation, worldRotationVec);

		SetRotationQuaternion(gameObject, worldRotation);
	}

	void Transforms::SetLocalRotationByDegrees(GameObject gameObject, float x, float y, float z)
	{
		SetLocalRotationByDegrees(gameObject, { x, y, z });
	}

	DirectX::XMFLOAT4 Transforms::GetRotationQuaternion(GameObject gameObject)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();
		return tc.Rotation;
	}

	DirectX::XMFLOAT4 Transforms::GetLocalRotationQuaternion(GameObject gameObject)
	{
		if (!HasParent(gameObject))
			return GetRotationQuaternion(gameObject);

		auto parentRotation = GetParent(gameObject).GetComponent<TransformComponent>().Rotation;
		auto parentRotationInverVec = DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentRotation));

		auto worldRotation = GetRotationQuaternion(gameObject);
		auto worldRotationVec = DirectX::XMLoadFloat4(&worldRotation);

		auto localRotationVec = DirectX::XMQuaternionMultiply(worldRotationVec, parentRotationInverVec);
		localRotationVec = DirectX::XMQuaternionNormalize(localRotationVec);

		DirectX::XMFLOAT4 localRotation{};
		DirectX::XMStoreFloat4(&localRotation, localRotationVec);

		return localRotation;
	}

	DirectX::XMFLOAT3 Transforms::GetRotationDegrees(GameObject gameObject)
	{
		return Math::QuaternionToDegrees(GetRotationQuaternion(gameObject));
	}

	DirectX::XMFLOAT3 Transforms::GetLocalRotationDegrees(GameObject gameObject)
	{
		return Math::QuaternionToDegrees(GetLocalRotationQuaternion(gameObject));
	}

	void Transforms::SetScale(GameObject gameObject, const DirectX::XMFLOAT3& scl)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();
		tc.Scale = scl;
	}

	void Transforms::SetScale(GameObject gameObject, float x, float y, float z)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();
		tc.Scale = { x, y, z };
	}

	void Transforms::Translate(GameObject gameObject, const DirectX::XMFLOAT3& move, eSpace space)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();

		if (space == eSpace::World)
		{
			tc.Position.x += move.x;
			tc.Position.y += move.y;
			tc.Position.z += move.z;
			return;
		}
		else
		{
			if (!HasParent(gameObject))
			{
				tc.Position.x += move.x;
				tc.Position.y += move.y;
				tc.Position.z += move.z;
				return;
			}
			else
			{
				auto parentTransform = GetTransform(GetParent(gameObject));
				auto parentInverseTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&parentTransform));

				auto worldMoveVec = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&move), parentInverseTransform);
				
				DirectX::XMFLOAT3 worldMove{};
				DirectX::XMStoreFloat3(&worldMove, worldMoveVec);

				tc.Position.x += worldMove.x;
				tc.Position.y += worldMove.y;
				tc.Position.z += worldMove.z;
				return;
			}
		}
	}

	void Transforms::Translate(GameObject gameObject, float x, float y, float z, eSpace space)
	{
		Translate(gameObject, { x, y, z }, space);
	}

	void Transforms::Rotate(GameObject gameObject, const DirectX::XMFLOAT3& degrees, eSpace space)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();

		auto radiansX = DirectX::XMConvertToRadians(degrees.x);
		auto radiansY = DirectX::XMConvertToRadians(degrees.y);
		auto radiansZ = DirectX::XMConvertToRadians(degrees.z);

		auto rotationVec = DirectX::XMQuaternionRotationRollPitchYaw(radiansX, radiansY, radiansZ);

		if (space == eSpace::World)
		{
			auto worldRotationVec = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&tc.Rotation), rotationVec);
			worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);
			DirectX::XMStoreFloat4(&tc.Rotation, worldRotationVec);
			return;
		}
		else
		{
			if (!HasParent(gameObject))
			{
				auto worldRotationVec = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&tc.Rotation), rotationVec);
				worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);
				DirectX::XMStoreFloat4(&tc.Rotation, worldRotationVec);
				return;
			}
			else
			{
				auto parentRotation = GetParent(gameObject).GetComponent<TransformComponent>().Rotation;
				auto parentRotationInverseVec = DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentRotation));

				auto localRotationVec = DirectX::XMQuaternionMultiply(rotationVec, parentRotationInverseVec);
				auto worldRotationVec = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&tc.Rotation), localRotationVec);
				worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);

				DirectX::XMStoreFloat4(&tc.Rotation, worldRotationVec);
				return;
			}
		}
	}

	void Transforms::Rotate(GameObject gameObject, float x, float y, float z, eSpace space)
	{
		Rotate(gameObject, { x,y,z }, space);
	}

	DirectX::XMFLOAT4X4 Transforms::GetTransform(GameObject gameObject)
	{
		if (!HasParent(gameObject))
			return GetLocalTransform(gameObject);

		auto& tc = gameObject.GetComponent<TransformComponent>();
		auto parentTransform = GetTransform(GetParent(gameObject));
		auto transformMat = DirectX::XMLoadFloat4x4(&parentTransform) * DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&tc.Scale))
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&tc.Rotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tc.Position));

		DirectX::XMFLOAT4X4 transform{};
		DirectX::XMStoreFloat4x4(&transform, transformMat);

		return transform;
	}

	DirectX::XMFLOAT4X4 Transforms::GetLocalTransform(GameObject gameObject)
	{
		auto& tc = gameObject.GetComponent<TransformComponent>();

		auto transformMat = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&tc.Scale))
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&tc.Rotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tc.Position));

		DirectX::XMFLOAT4X4 transform{};
		DirectX::XMStoreFloat4x4(&transform, transformMat);

		return transform;
	}

	void Transforms::SetParent(GameObject child, GameObject parent)
	{
		RemoveParent(child);

		s_Hierarchy[child].Parent = parent;
		s_Hierarchy[parent].Children.push_back(child);
	}

	GameObject Transforms::GetParent(GameObject gameObject)
	{
		auto it = s_Hierarchy.find(gameObject);
		DV_ASSERT(Transforms, it != s_Hierarchy.end());

		return it->second.Parent;
	}
	
	void Transforms::RemoveParent(GameObject child)
	{
		auto& node = s_Hierarchy[child];
		if (node.Parent != GameObject())
		{
			auto& parentNode = s_Hierarchy[node.Parent];
			parentNode.Children.erase(
				std::remove(parentNode.Children.begin(), parentNode.Children.end(), child),
				parentNode.Children.end()
			);
			node.Parent = GameObject();
		}
	}

	std::vector<GameObject> Transforms::GetChildren(GameObject gameObject)
	{
		auto it = s_Hierarchy.find(gameObject);
		if (it != s_Hierarchy.end())
			return it->second.Children;

		return {};
	}

	std::vector<GameObject> Transforms::GetRootNodes(std::vector<GameObject> entities)
	{
		std::vector<GameObject> rootNodes{};

		for (auto& gameObject : entities)
		{
			auto it = s_Hierarchy.find(gameObject);
			if(it == s_Hierarchy.end() || it->second.Parent == GameObject())
				rootNodes.push_back(gameObject);
		}

		return rootNodes;
	}
	
	bool Transforms::HasParent(GameObject gameObject)
	{
		auto it = s_Hierarchy.find(gameObject);
		DV_ASSERT(Transforms, it != s_Hierarchy.end());

		return it->second.Parent;
	}

	bool Transforms::HasChildren(GameObject gameObject)
	{
		auto it = s_Hierarchy.find(gameObject);
		DV_ASSERT(Transform, it != s_Hierarchy.end());

		return !it->second.Children.empty();
	}
	
	void Transforms::Serialize(YAML::Emitter& out, const std::vector<GameObject>& gameObjects)
	{
		out << YAML::Key << "TransformHierarchy";
		out << YAML::Value << YAML::BeginSeq;
		
		for (auto& gameObject : gameObjects)
		{
			// 직렬화 대상은 부모의 ID로 충분하다.
			//auto parentID = HasParent(gameObject) ? GetParent()
		}
	}

	bool Transforms::Deserialize(YAML::Emitter& out)
	{
		return false;
	}
}