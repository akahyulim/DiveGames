#include "stdafx.h"
#include "DvTransforms.h"
#include "core/CoreDefs.h"
#include "math/Math.h"
#include "world/World.h"

namespace Dive
{
	void DvTransforms::Initailize()
	{
	}
	
	void DvTransforms::Shutdown()
	{
	}

	void DvTransforms::Update(std::vector<TransformComponent*> transformComponents)
	{
		for (auto tc : transformComponents)
		{
			if (tc->Dirty)
			{
				tc->WorldMatrix = calcWorldMatrix(*tc);
				tc->Dirty = false;
			}
		}
	}

	DirectX::XMFLOAT3 DvTransforms::GetWorldPosition(const TransformComponent& tc)
	{
		if(!tc.Parent)
			return tc.Position;

		auto& parent = tc.Parent->GetComponent<TransformComponent>();
		auto parentMatrix = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);
		auto localPosition = DirectX::XMLoadFloat3(&tc.Position);
		
		auto worldPosition = DirectX::XMVector3TransformCoord(localPosition, parentMatrix);

		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, worldPosition);

		return result;
	}

	void DvTransforms::SetWorldPosition(TransformComponent& tc, const DirectX::XMFLOAT3& pos)
	{
		if (!tc.Parent)
		{
			SetLocalPosition(tc, pos);
			return;
		}

		auto& parent = tc.Parent->GetComponent<TransformComponent>();
		auto parentMatrix = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);

		auto determinant = DirectX::XMMatrixDeterminant(parentMatrix);
		if (DirectX::XMVector3Equal(determinant, DirectX::XMVectorZero()))
		{
			DV_LOG(DvTransform, warn, "역행렬이 존재하지 않음");
			return;
		}

		auto localPos = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&pos), DirectX::XMMatrixInverse(nullptr, parentMatrix));
		
		DirectX::XMStoreFloat3(&tc.Position, localPos);
		tc.Dirty = true;
	}

	void DvTransforms::SetLocalPosition(TransformComponent& tc, const DirectX::XMFLOAT3& pos)
	{
		tc.Position = pos;
		tc.Dirty = true;
	}

	DirectX::XMFLOAT4 DvTransforms::GetWorldRotationQuaternion(const TransformComponent& tc)
	{
		if (!tc.Parent)
			return tc.Rotation;

		auto& parent = tc.Parent->GetComponent<TransformComponent>();
		auto parentWorldMat = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);

		DirectX::XMVECTOR scale, rotation, translation;
		DirectX::XMMatrixDecompose(&scale, &rotation, &translation, parentWorldMat);

		auto localRotation = DirectX::XMLoadFloat4(&tc.Rotation);
		auto worldRotation = DirectX::XMQuaternionMultiply(rotation, localRotation);

		DirectX::XMFLOAT4 result;
		DirectX::XMStoreFloat4(&result, worldRotation);

		return result;
	}

	void DvTransforms::SetWorldRotationQuaternion(TransformComponent& tc, const DirectX::XMFLOAT4& rot)
	{
		if (!tc.Parent)
		{
			SetLocalRotationQuaternion(tc, rot);
			return;
		}

		auto& parent = tc.Parent->GetComponent<TransformComponent>();
		auto parentRotation = DirectX::XMLoadFloat4(&parent.Rotation);
		auto worldRotation = DirectX::XMLoadFloat4(&rot);

		auto localRotation = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(parentRotation), worldRotation);
	
		DirectX::XMStoreFloat4(&tc.Rotation, localRotation);
		tc.Dirty = true;
	}

	void DvTransforms::SetLocalRotationQuaternion(TransformComponent& tc, const DirectX::XMFLOAT4& rot)
	{
		tc.Rotation = rot;
		tc.Dirty = true;
	}

	DirectX::XMFLOAT3 DvTransforms::GetWorldRotationDegrees(const TransformComponent& tc)
	{
		auto radians = GetWorldRotationRadians(tc);
		return {
			DirectX::XMConvertToDegrees(radians.x),
			DirectX::XMConvertToDegrees(radians.y),
			DirectX::XMConvertToDegrees(radians.z)
		};
	}

	void DvTransforms::SetWorldRotationDegrees(TransformComponent& tc, const DirectX::XMFLOAT3& rot)
	{
		SetWorldRotationRadians(tc,
			{ 
				DirectX::XMConvertToRadians(rot.x),
				DirectX::XMConvertToRadians(rot.y),
				DirectX::XMConvertToRadians(rot.z) 
			}
		);
	}

	DirectX::XMFLOAT3 DvTransforms::GetLocalRotationDegrees(const TransformComponent& tc)
	{
		auto radians = GetLocalRotationRadians(tc);
		return {
			DirectX::XMConvertToDegrees(radians.x),
			DirectX::XMConvertToDegrees(radians.y),
			DirectX::XMConvertToDegrees(radians.z)
		};
	}

	void DvTransforms::SetLocaldRotationDegrees(TransformComponent& tc, const DirectX::XMFLOAT3& rot)
	{
		SetLocalRotationRadians(tc,
			{ DirectX::XMConvertToRadians(rot.x),
			DirectX::XMConvertToRadians(rot.y),
			DirectX::XMConvertToRadians(rot.z) }
		);
	}

	DirectX::XMFLOAT3 DvTransforms::GetWorldRotationRadians(const TransformComponent& tc)
	{
		if (!tc.Parent)
			return GetLocalRotationRadians(tc);

		return Math::QuaternionToRadians(GetWorldRotationQuaternion(tc));
	}

	void DvTransforms::SetWorldRotationRadians(TransformComponent& tc, const DirectX::XMFLOAT3& rot)
	{
		if (!tc.Parent)
		{
			SetLocalRotationRadians(tc, rot);
			return;
		}

		auto& parent = tc.Parent->GetComponent<TransformComponent>();
		auto parentRotation = DirectX::XMLoadFloat4(&parent.Rotation);
		auto localRotation = DirectX::XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);

		auto worldRotation = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(parentRotation), localRotation);

		DirectX::XMStoreFloat4(&tc.Rotation, worldRotation);
		tc.Dirty = true;
	}

	DirectX::XMFLOAT3 DvTransforms::GetLocalRotationRadians(const TransformComponent& tc)
	{
		return Math::QuaternionToRadians(tc.Rotation);
	}

	void DvTransforms::SetLocalRotationRadians(TransformComponent& tc, const DirectX::XMFLOAT3& rot)
	{
		auto localRotation = DirectX::XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);
		DirectX::XMStoreFloat4(&tc.Rotation, localRotation);
		tc.Dirty = true;
	}

	DirectX::XMFLOAT3 DvTransforms::GetWorldScale(const TransformComponent& tc)
	{
		if (!tc.Parent)
			return tc.Scale;

		auto& parent = tc.Parent->GetComponent<TransformComponent>();
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

	void DvTransforms::SetWorldScale(TransformComponent& tc, const DirectX::XMFLOAT3& scale)
	{
		if (!tc.Parent)
		{
			SetLocalScale(tc, scale);
			return;
		}

		auto& parent = tc.Parent->GetComponent<TransformComponent>();
		auto parentScale = GetWorldScale(parent);
		tc.Scale = { 
			scale.x / std::max(parentScale.x, 0.0001f),
			scale.y / std::max(parentScale.y, 0.0001f),
			scale.z / std::max(parentScale.z, 0.0001f)
		};
		tc.Dirty = true;
	}

	void DvTransforms::SetLocalScale(TransformComponent& tc, const DirectX::XMFLOAT3& scale)
	{
		tc.Scale = scale;
		tc.Dirty = true;
	}

	void DvTransforms::Translate(TransformComponent& tc, const DirectX::XMFLOAT3& move, eSpace space)
	{
		if (move.x == 0.0f && move.y == 0.0f && move.z == 0.0f)
			return;

		auto delta = DirectX::XMLoadFloat3(&move);

		if (space == eSpace::Self && tc.Parent)
		{
			auto& parent = tc.Parent->GetComponent<TransformComponent>();
			auto parentWorldRotation = GetWorldRotationQuaternion(parent);
			delta = DirectX::XMVector3Rotate(delta, DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentWorldRotation)));
		}

		tc.Position.x += DirectX::XMVectorGetX(delta);
		tc.Position.y += DirectX::XMVectorGetY(delta);
		tc.Position.z += DirectX::XMVectorGetZ(delta);

		tc.Dirty = true;
	}

	void DvTransforms::RotateQuaternion(TransformComponent& tc, const DirectX::XMFLOAT4& quat, eSpace space)
	{
		auto delta = DirectX::XMLoadFloat4(&quat);

		if (DirectX::XMQuaternionIsIdentity(delta))
			return;

		auto localRotation = DirectX::XMLoadFloat4(&tc.Rotation);

		if (space == eSpace::Self && tc.Parent)
		{
			auto& parent = tc.Parent->GetComponent<TransformComponent>();
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

	void DvTransforms::RotateDegrees(TransformComponent& tc, const DirectX::XMFLOAT3& degrees, eSpace space)
	{
		RotateRadians(
			tc,
			{
				DirectX::XMConvertToRadians(degrees.x),
				DirectX::XMConvertToRadians(degrees.y),
				DirectX::XMConvertToRadians(degrees.z)
			},
			space
		);
	}

	void DvTransforms::RotateRadians(TransformComponent& tc, const DirectX::XMFLOAT3& radians, eSpace space)
	{
		auto delta = DirectX::XMQuaternionRotationRollPitchYaw(radians.x, radians.y, radians.z);

		if (DirectX::XMVectorGetX(DirectX::XMVectorIsNaN(delta)) ||
			DirectX::XMQuaternionEqual(delta, DirectX::XMQuaternionIdentity()))
			return;

		auto localRotation = DirectX::XMLoadFloat4(&tc.Rotation);

		if (space == eSpace::Self && tc.Parent)
		{
			auto& parent = tc.Parent->GetComponent<TransformComponent>();
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

	DirectX::XMFLOAT4X4 DvTransforms::GetLocalMatrix(const TransformComponent& tc)
	{
		auto localMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&tc.Scale))
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&tc.Rotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tc.Position));

		DirectX::XMFLOAT4X4 result;
		DirectX::XMStoreFloat4x4(&result, localMatrix);

		return result;
	}

	Entity* DvTransforms::GetParent(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		return tc.Parent;
	}

	// 현재 Entity를 다루는 방식이 중구난방이다.
	void DvTransforms::SetParent(Entity child, Entity parent)
	{
		// 부모 대상이 자식일 순 없다.
		if (parent.GetUUID() !=  0 && IsChildOf(child, parent))
			return;

		auto& tcChild = child.GetComponent<TransformComponent>();
		if (tcChild.Parent)
		{
			auto& tcOldParent = tcChild.Parent->GetComponent<TransformComponent>();
			tcOldParent.Children.erase(
				std::remove(
					tcOldParent.Children.begin(),
					tcOldParent.Children.end(),
					&child),
				tcOldParent.Children.end()
			);
		}

		if (parent.GetUUID() != 0)
		{
			auto& tcParent = parent.GetComponent<TransformComponent>();
			tcParent.Children.push_back(&child);
		}

		tcChild.Parent = &parent;
		tcChild.Dirty = true;
	}

	bool DvTransforms::IsChildOf(Entity& parent, Entity& target)
	{
		auto& tcParent = parent.GetComponent<TransformComponent>();
		for (auto child : tcParent.Children)
		{
			if (child == &target)
				return true;

			if (IsChildOf(*child, target))
				return true;
		}

		return false;
	}

	std::vector<Entity*>& DvTransforms::GetChildren(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		return tc.Children;
	}

	bool DvTransforms::HasParent(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		return tc.Parent != nullptr;
	}

	bool DvTransforms::HasChild(Entity& entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		return !tc.Children.empty();
	}

	// GetComponent때문에 상수 참조로 전달받지 못한다.
	// 이런 메서드가 몇개 더 있다.
	// 근데 매개변수룰 전달하는 World::GetAllEntities 역시 지역변수로 만들어서 참조로 리턴할 수 없다.
	std::vector<Entity> DvTransforms::GetRootNodes(std::vector<Entity> gameObjects)
	{
		std::vector<Entity> rootNodes;
		for (auto& entity : gameObjects)
		{
			if (!entity.GetComponent<TransformComponent>().Parent)
				rootNodes.emplace_back(entity);
		}

		return rootNodes;
	}

	/*
	TransformComponent* DvTransforms::GetParent(const TransformComponent& tc)
	{
		return &tc.Parent->GetComponent<TransformComponent>();
	}

	void DvTransforms::SetParent(TransformComponent& tc, TransformComponent* parent)
	{
		// 자식이 부모가 될 수 없다.
		if (parent && IsChildOf(tc, *parent))
			return;

		if (tc.Parent)
		{
			auto& parent = tc.Parent->GetComponent<TransformComponent>();
			parent.Children.erase(
				std::remove(
					parent.Children.begin(), 
					parent.Children.end(), 
					&tc), 
				parent.Children.end()
			);
		}

		if (parent)
			parent->Children.push_back(&tc);

		tc.Parent = parent;
		tc.Dirty = true;
	}

	bool DvTransforms::IsChildOf(const TransformComponent& parent, const TransformComponent& tc)
	{
		for (auto child : parent.Children)
		{
			if (child == &tc)
				return true;

			if (IsChildOf(*child, tc))
				return true;
		}

		return false;
	}

	std::vector<Entity*>& DvTransforms::GetChildren(TransformComponent& tc)
	{
		return tc.Children;
	}

	bool DvTransforms::HasParent(const TransformComponent& tc)
	{
		return tc.Parent != nullptr;
	}

	bool DvTransforms::HasChild(const TransformComponent& tc)
	{
		return !tc.Children.empty();
	}
	*/
	DirectX::XMFLOAT4X4 DvTransforms::calcWorldMatrix(const TransformComponent& tc)
	{
		auto localMatrix = GetLocalMatrix(tc);

		if (tc.Parent)
		{
			auto& parent = tc.Parent->GetComponent<TransformComponent>();
			auto parentMatrix = DirectX::XMLoadFloat4x4(&parent.WorldMatrix);
			auto worldMatrix = DirectX::XMMatrixMultiply(parentMatrix, DirectX::XMLoadFloat4x4(&localMatrix));

			DirectX::XMFLOAT4X4 result;
			DirectX::XMStoreFloat4x4(&result, worldMatrix);

			return result;
		}

		return localMatrix;
	}
}