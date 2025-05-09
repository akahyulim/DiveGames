#include "stdafx.h"
#include "transforms.h"
#include "core/CoreDefs.h"
#include "math/Math.h"
#include "world/Components.h"

namespace Dive
{
	std::unordered_map<entt::entity, Transforms::HierarchyNode> Transforms::s_Hierarchy;

	void Transforms::Initialize()
	{
	}
	
	void Transforms::Shutdown()
	{
	}

	void Transforms::Tick(std::vector<Entity>& entities)
	{
		for (auto& entity : entities)
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			if (tc.Dirty)
			{
				tc.WorldMatrix = calcWorldMatrix(entity);
				tc.Dirty = false;
			}
		}	
	}
	
	DirectX::XMFLOAT3 Transforms::GetWorldPosition(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();

		if (!HasParent(entity))
			return tc.Position;

		auto& parent = GetParent(entity).GetComponent<TransformComponent>();
		auto parentMatrix = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);
		auto localPosition = DirectX::XMLoadFloat3(&tc.Position);

		auto worldPosition = DirectX::XMVector3TransformCoord(localPosition, parentMatrix);

		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, worldPosition);

		return result;
	}

	void Transforms::SetWorldPosition(Entity& entity, const DirectX::XMFLOAT3& pos)
	{
		if (!HasParent(entity))
		{
			SetLocalPosition(entity, pos);
			return;
		}

		auto& parent = GetParent(entity).GetComponent<TransformComponent>();
		auto parentMatrix = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);

		auto determinant = DirectX::XMMatrixDeterminant(parentMatrix);
		if (DirectX::XMVector3Equal(determinant, DirectX::XMVectorZero()))
		{
			DV_LOG(DvTransform, warn, "역행렬이 존재하지 않음");
			return;
		}

		auto localPos = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&pos), DirectX::XMMatrixInverse(nullptr, parentMatrix));

		auto& tc = entity.GetComponent<TransformComponent>();
		DirectX::XMStoreFloat3(&tc.Position, localPos);
		tc.Dirty = true;
	}

	void Transforms::SetLocalPosition(Entity& entity, const DirectX::XMFLOAT3& pos)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Position = pos;
		tc.Dirty = true;
	}

	DirectX::XMFLOAT4 Transforms::GetWorldRotationQuaternion(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();

		if (!HasParent(entity))
			return tc.Rotation;

		auto& parent = GetParent(entity).GetComponent<TransformComponent>();
		auto parentWorldMat = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);

		DirectX::XMVECTOR scale, rotation, translation;
		DirectX::XMMatrixDecompose(&scale, &rotation, &translation, parentWorldMat);

		auto localRotation = DirectX::XMLoadFloat4(&tc.Rotation);
		auto worldRotation = DirectX::XMQuaternionMultiply(rotation, localRotation);

		DirectX::XMFLOAT4 result;
		DirectX::XMStoreFloat4(&result, worldRotation);

		return result;
	}

	void Transforms::SetWorldRotationQuaternion(Entity& entity, const DirectX::XMFLOAT4& rot)
	{
		if (!HasParent(entity))
		{
			SetLocalRotationQuaternion(entity, rot);
			return;
		}

		auto& parent = GetParent(entity).GetComponent<TransformComponent>();
		auto parentRotation = DirectX::XMLoadFloat4(&parent.Rotation);
		auto worldRotation = DirectX::XMLoadFloat4(&rot);

		auto localRotation = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(parentRotation), worldRotation);

		auto& tc = entity.GetComponent<TransformComponent>();
		DirectX::XMStoreFloat4(&tc.Rotation, localRotation);
		tc.Dirty = true;
	}

	void Transforms::SetLocalRotationQuaternion(Entity& entity, const DirectX::XMFLOAT4& rot)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Rotation = rot;
		tc.Dirty = true;
	}

	DirectX::XMFLOAT3 Transforms::GetWorldRotationDegrees(Entity& entity)
	{
		auto radians = GetWorldRotationRadians(entity);
		return {
			DirectX::XMConvertToDegrees(radians.x),
			DirectX::XMConvertToDegrees(radians.y),
			DirectX::XMConvertToDegrees(radians.z)
		};
	}

	void Transforms::SetWorldRotationDegrees(Entity& entity, const DirectX::XMFLOAT3& rot)
	{
		SetWorldRotationRadians(entity,
			{
				DirectX::XMConvertToRadians(rot.x),
				DirectX::XMConvertToRadians(rot.y),
				DirectX::XMConvertToRadians(rot.z)
			}
		);
	}

	DirectX::XMFLOAT3 Transforms::GetLocalRotationDegrees(Entity& entity)
	{
		auto radians = GetLocalRotationRadians(entity);
		return {
			DirectX::XMConvertToDegrees(radians.x),
			DirectX::XMConvertToDegrees(radians.y),
			DirectX::XMConvertToDegrees(radians.z)
		};
	}

	void Transforms::SetLocaldRotationDegrees(Entity& entity, const DirectX::XMFLOAT3& rot)
	{
		SetLocalRotationRadians(entity,
			{ DirectX::XMConvertToRadians(rot.x),
			DirectX::XMConvertToRadians(rot.y),
			DirectX::XMConvertToRadians(rot.z) }
		);
	}

	DirectX::XMFLOAT3 Transforms::GetWorldRotationRadians(Entity& entity)
	{
		if (!HasParent(entity))
			return GetLocalRotationRadians(entity);

		return Math::QuaternionToRadians(GetWorldRotationQuaternion(entity));
	}

	void Transforms::SetWorldRotationRadians(Entity& entity, const DirectX::XMFLOAT3& rot)
	{
		if (!HasParent(entity))
		{
			SetLocalRotationRadians(entity, rot);
			return;
		}

		auto& parent = GetParent(entity).GetComponent<TransformComponent>();
		auto parentRotation = DirectX::XMLoadFloat4(&parent.Rotation);
		auto localRotation = DirectX::XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);

		auto worldRotation = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(parentRotation), localRotation);

		auto& tc = entity.GetComponent<TransformComponent>();
		DirectX::XMStoreFloat4(&tc.Rotation, worldRotation);
		tc.Dirty = true;
	}

	DirectX::XMFLOAT3 Transforms::GetLocalRotationRadians(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		return Math::QuaternionToRadians(tc.Rotation);
	}

	void Transforms::SetLocalRotationRadians(Entity& entity, const DirectX::XMFLOAT3& rot)
	{
		auto localRotation = DirectX::XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);

		auto& tc = entity.GetComponent<TransformComponent>();
		DirectX::XMStoreFloat4(&tc.Rotation, localRotation);
		tc.Dirty = true;
	}

	DirectX::XMFLOAT3 Transforms::GetWorldScale(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		if (!HasParent(entity))
			return tc.Scale;

		auto& parent = GetParent(entity).GetComponent<TransformComponent>();
		auto parentWorldMatrix = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);

		DirectX::XMFLOAT3 parentWorldScale = {
			DirectX::XMVectorGetX(DirectX::XMVector3Length(parentWorldMatrix.r[0])),
			DirectX::XMVectorGetY(DirectX::XMVector3Length(parentWorldMatrix.r[1])),
			DirectX::XMVectorGetZ(DirectX::XMVector3Length(parentWorldMatrix.r[2])),
		};

		return {
			parentWorldScale.x * tc.Scale.x,
			parentWorldScale.y * tc.Scale.y,
			parentWorldScale.z * tc.Scale.z
		};
	}

	void Transforms::SetWorldScale(Entity& entity, const DirectX::XMFLOAT3& scale)
	{
		if (!HasParent(entity))
		{
			SetLocalScale(entity, scale);
			return;
		}

		auto parent = GetParent(entity);
		auto parentScale = GetWorldScale(parent);

		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Scale = {
			scale.x / std::max(parentScale.x, 0.0001f),
			scale.y / std::max(parentScale.y, 0.0001f),
			scale.z / std::max(parentScale.z, 0.0001f)
		};
		tc.Dirty = true;
	}

	void Transforms::SetLocalScale(Entity& entity, const DirectX::XMFLOAT3& scale)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Scale = scale;
		tc.Dirty = true;
	}

	void Transforms::Translate(Entity& entity, const DirectX::XMFLOAT3& move, eSpace space)
	{
		if (move.x == 0.0f && move.y == 0.0f && move.z == 0.0f)
			return;

		auto delta = DirectX::XMLoadFloat3(&move);

		if (space == eSpace::Self && HasParent(entity))
		{
			auto parent = GetParent(entity);
			auto parentWorldRotation = GetWorldRotationQuaternion(parent);
			delta = DirectX::XMVector3Rotate(delta, DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentWorldRotation)));
		}

		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Position.x += DirectX::XMVectorGetX(delta);
		tc.Position.y += DirectX::XMVectorGetY(delta);
		tc.Position.z += DirectX::XMVectorGetZ(delta);

		tc.Dirty = true;
	}

	void Transforms::RotateQuaternion(Entity& entity, const DirectX::XMFLOAT4& quat, eSpace space)
	{
		auto delta = DirectX::XMLoadFloat4(&quat);

		if (DirectX::XMQuaternionIsIdentity(delta))
			return;

		auto& tc = entity.GetComponent<TransformComponent>();
		auto localRotation = DirectX::XMLoadFloat4(&tc.Rotation);

		if (space == eSpace::Self && HasParent(entity))
		{
			auto parent = GetParent(entity);
			auto parentWorldRotation = GetWorldRotationQuaternion(parent);
			delta = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentWorldRotation)), delta);
		}

		auto newRotation = DirectX::XMQuaternionMultiply(localRotation, delta);
		newRotation = DirectX::XMQuaternionNormalize(newRotation);

		if (!DirectX::XMQuaternionEqual(newRotation, localRotation))
		{
			DirectX::XMStoreFloat4(&tc.Rotation, newRotation);
			tc.Dirty = true;
		}
	}

	void Transforms::RotateDegrees(Entity& entity, const DirectX::XMFLOAT3& degrees, eSpace space)
	{
		RotateRadians(
			entity,
			{
				DirectX::XMConvertToRadians(degrees.x),
				DirectX::XMConvertToRadians(degrees.y),
				DirectX::XMConvertToRadians(degrees.z)
			},
			space
		);
	}

	void Transforms::RotateRadians(Entity& entity, const DirectX::XMFLOAT3& radians, eSpace space)
	{
		auto delta = DirectX::XMQuaternionRotationRollPitchYaw(radians.x, radians.y, radians.z);

		if (DirectX::XMVectorGetX(DirectX::XMVectorIsNaN(delta)) ||
			DirectX::XMQuaternionEqual(delta, DirectX::XMQuaternionIdentity()))
			return;

		auto& tc = entity.GetComponent<TransformComponent>();
		auto localRotation = DirectX::XMLoadFloat4(&tc.Rotation);

		if (space == eSpace::Self && HasParent(entity))
		{
			auto parent = GetParent(entity);
			auto parentWorldRotation = GetWorldRotationQuaternion(parent);
			delta = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentWorldRotation)), delta);
		}

		auto newRotation = DirectX::XMQuaternionMultiply(localRotation, delta);
		newRotation = DirectX::XMQuaternionNormalize(newRotation);

		if (!DirectX::XMQuaternionEqual(newRotation, localRotation))
		{
			DirectX::XMStoreFloat4(&tc.Rotation, newRotation);
			tc.Dirty = true;
		}
	}

	DirectX::XMFLOAT4X4 Transforms::GetLocalMatrix(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		auto localMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&tc.Scale))
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&tc.Rotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tc.Position));

		DirectX::XMFLOAT4X4 result;
		DirectX::XMStoreFloat4x4(&result, localMatrix);

		return result;
	}

	// 이슈1. 부모로부터 독립하면 로컬좌표는 기존 월드좌표와 동일해진다.
	// 이슈2. 부모가 바뀌면 기존 월드 좌표는 유지되고 로컬좌표는 부모의 좌표계를 기준으로 변경된다.
	void Transforms::SetParent(Entity child, Entity parent)
	{
		DV_ASSERT(Transforms, child);

		// 새로운 부모가 자식인 경우 확인
		if (parent && IsChildOf(child, parent))
			return;

		RemoveParent(child);

		s_Hierarchy[child].Parent = parent;
		s_Hierarchy[parent].Children.push_back(child);

		child.GetComponent<TransformComponent>().Dirty = true;
	}

	Entity Transforms::GetParent(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		auto it = s_Hierarchy.find(entity);
		if (it != s_Hierarchy.end())
			return it->second.Parent;

		return {};
	}
	
	void Transforms::RemoveParent(Entity child)
	{
		DV_ASSERT(Transforms, child);

		auto& node = s_Hierarchy[child];
		if (node.Parent != Entity())
		{
			auto& parentNode = s_Hierarchy[node.Parent];
			parentNode.Children.erase(
				std::remove(parentNode.Children.begin(), parentNode.Children.end(), child),
				parentNode.Children.end()
			);
			node.Parent = Entity();
		}
	}

	std::vector<Entity> Transforms::GetChildren(Entity parent)
	{
		DV_ASSERT(Transforms, parent);

		auto it = s_Hierarchy.find(parent);
		if (it != s_Hierarchy.end())
			return it->second.Children;

		return {};
	}

	// 이슈: 재귀구조로 구현하여 하위 자식들도 대상이 될 수 있어야 한다.
	bool Transforms::IsChildOf(Entity parent, Entity other)
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

	std::vector<Entity> Transforms::GetRootNodes(std::vector<Entity> gameObjects)
	{
		std::vector<Entity> rootNodes{};

		for (auto& entity : gameObjects)
		{
			auto it = s_Hierarchy.find(entity);
			if(it == s_Hierarchy.end() || it->second.Parent == Entity())
				rootNodes.push_back(entity);
		}

		return rootNodes;
	}
	
	bool Transforms::HasParent(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		auto it = s_Hierarchy.find(entity);
		if (it == s_Hierarchy.end() || !it->second.Parent)
			return false;

		return true;
	}

	bool Transforms::HasChildren(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		auto it = s_Hierarchy.find(entity);
		if (it == s_Hierarchy.end() || it->second.Children.empty())
			return false;

		return true;
	}

	DirectX::XMFLOAT4X4 Transforms::calcWorldMatrix(Entity& entity)
	{
		auto matrix = GetLocalMatrix(entity);

		if (HasParent(entity))
		{
			auto& parent = GetParent(entity).GetComponent<TransformComponent>();
			auto parentMatrix = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);
			auto worldMatrix = DirectX::XMMatrixMultiply(parentMatrix, DirectX::XMLoadFloat4x4(&matrix));

			DirectX::XMFLOAT4X4 result;
			DirectX::XMStoreFloat4x4(&result, worldMatrix);

			return result;
		}
	
		return matrix;
	}

	void Transforms::update(Entity& entity)
	{
		if (HasParent(entity))
		{

		}
		else
		{

		}
	}
}