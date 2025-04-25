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

	void Transforms::Tick(std::vector<TransformComponent*>& transformList)
	{
		for (auto tc : transformList)
		{
			if (tc->Dirty)
			{
				tc->WorldMatrix = tc->GetWorldMatrix();
				tc->Dirty = false;
			}
		}	
	}
	void Transforms::SetPosition(GameObject gameObject, const DirectX::XMFLOAT3& pos)
	{
		DV_ASSERT(Transforms, gameObject);

		if (!HasParent(gameObject)) 
		{
			SetLocalPosition(gameObject, pos);
			return;
		}

		auto& tc = gameObject.GetComponent<TransformComponent>();
		auto parentWorldMat = DirectX::XMLoadFloat4x4(&tc.Parent->WorldMatrix);

		auto localPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos),
			DirectX::XMMatrixInverse(nullptr, parentWorldMat));

		DirectX::XMFLOAT3 newPos;
		DirectX::XMStoreFloat3(&newPos, localPos);

		SetLocalPosition(gameObject, newPos);
	}

	void Transforms::SetPosition(GameObject gameObject, float x, float y, float z)
	{
		DV_ASSERT(Transforms, gameObject);

		SetPosition(gameObject, { x, y, z });
	}

	void Transforms::SetLocalPosition(GameObject gameObject, const DirectX::XMFLOAT3& pos)
	{
		DV_ASSERT(Transforms, gameObject);

		auto& tc = gameObject.GetComponent<TransformComponent>();
		tc.Position = pos;
		tc.MarkDirty();
	}

	void Transforms::SetLocalPosition(GameObject gameObject, float x, float y, float z)
	{
		DV_ASSERT(Transforms, gameObject);

		SetLocalPosition(gameObject, { x, y, z });
	}

	DirectX::XMFLOAT3 Transforms::GetPosition(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

		auto& tc = gameObject.GetComponent<TransformComponent>();
		return tc.Position;
	}

	DirectX::XMFLOAT3 Transforms::GetLocalPosition(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, gameObject);

		auto& tc = gameObject.GetComponent<TransformComponent>();
		tc.Rotation = rot;
	}

	void Transforms::SetRotationByDegrees(GameObject gameObject, const DirectX::XMFLOAT3& degrees)
	{
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, gameObject);

		SetRotationByDegrees(gameObject, { x, y, z });
	}

	void Transforms::SetLocalRotationQuaternion(GameObject gameObject, const DirectX::XMFLOAT4& rot)
	{
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, gameObject);

		SetLocalRotationByDegrees(gameObject, { x, y, z });
	}

	DirectX::XMFLOAT4 Transforms::GetRotationQuaternion(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

		auto& tc = gameObject.GetComponent<TransformComponent>();
		return tc.Rotation;
	}

	DirectX::XMFLOAT4 Transforms::GetLocalRotationQuaternion(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, gameObject);

		return Math::QuaternionToDegrees(GetRotationQuaternion(gameObject));
	}

	DirectX::XMFLOAT3 Transforms::GetLocalRotationDegrees(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

		return Math::QuaternionToDegrees(GetLocalRotationQuaternion(gameObject));
	}

	void Transforms::SetScale(GameObject gameObject, const DirectX::XMFLOAT3& scl)
	{
		DV_ASSERT(Transforms, gameObject);

		auto& tc = gameObject.GetComponent<TransformComponent>();
		tc.Scale = scl;
	}

	void Transforms::SetScale(GameObject gameObject, float x, float y, float z)
	{
		DV_ASSERT(Transforms, gameObject);

		auto& tc = gameObject.GetComponent<TransformComponent>();
		tc.Scale = { x, y, z };
	}

	void Transforms::Translate(GameObject gameObject, const DirectX::XMFLOAT3& move, eSpace space)
	{
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, gameObject);

		Translate(gameObject, { x, y, z }, space);
	}

	void Transforms::Rotate(GameObject gameObject, const DirectX::XMFLOAT3& degrees, eSpace space)
	{
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, gameObject);

		Rotate(gameObject, { x,y,z }, space);
	}

	DirectX::XMFLOAT4X4 Transforms::GetTransform(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, gameObject);

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
		DV_ASSERT(Transforms, child);

		// 새로운 부모가 자식인 경우 확인
		if (parent && IsChildOf(child, parent))
			return;

		RemoveParent(child);

		s_Hierarchy[child].Parent = parent;
		s_Hierarchy[parent].Children.push_back(child);
	}

	GameObject Transforms::GetParent(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

		auto it = s_Hierarchy.find(gameObject);
		if (it != s_Hierarchy.end())
			return it->second.Parent;

		return {};
	}
	
	void Transforms::RemoveParent(GameObject child)
	{
		DV_ASSERT(Transforms, child);

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

	std::vector<GameObject> Transforms::GetChildren(GameObject parent)
	{
		DV_ASSERT(Transforms, parent);

		auto it = s_Hierarchy.find(parent);
		if (it != s_Hierarchy.end())
			return it->second.Children;

		return {};
	}

	bool Transforms::IsChildOf(GameObject parent, GameObject other)
	{
		DV_ASSERT(Transforms, parent);
		DV_ASSERT(Transforms, other);

		if (HasChildren(parent))
		{
			auto& children = s_Hierarchy[parent].Children;
			for (auto& child : children)
			{
				if (child == other || IsChildOf(child, other))
					return true;
			}
		}

		return false;
	}

	std::vector<GameObject> Transforms::GetRootNodes(std::vector<GameObject> gameObjects)
	{
		std::vector<GameObject> rootNodes{};

		for (auto& gameObject : gameObjects)
		{
			auto it = s_Hierarchy.find(gameObject);
			if(it == s_Hierarchy.end() || it->second.Parent == GameObject())
				rootNodes.push_back(gameObject);
		}

		return rootNodes;
	}
	
	bool Transforms::HasParent(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

		auto it = s_Hierarchy.find(gameObject);
		if (it == s_Hierarchy.end() || !it->second.Parent)
			return false;

		return true;
	}

	bool Transforms::HasChildren(GameObject gameObject)
	{
		DV_ASSERT(Transforms, gameObject);

		auto it = s_Hierarchy.find(gameObject);
		if (it == s_Hierarchy.end() || it->second.Children.empty())
			return false;

		return true;
	}
}