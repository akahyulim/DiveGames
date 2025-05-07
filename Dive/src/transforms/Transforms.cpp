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
				tc->WorldMatrix;
				tc->Dirty = false;
			}
		}	
	}
	void Transforms::SetPosition(Entity entity, const DirectX::XMFLOAT3& pos)
	{
		DV_ASSERT(Transforms, entity);

		if (!HasParent(entity)) 
		{
			SetLocalPosition(entity, pos);
			return;
		}

		// 이건 손대지 않은 것 같은데...
		/*
		auto& tc = entity.GetComponent<TransformComponent>();
		auto parentWorldMat = DirectX::XMLoadFloat4x4(&tc.Parent->WorldMatrix);

		auto localPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos),
			DirectX::XMMatrixInverse(nullptr, parentWorldMat));

		DirectX::XMFLOAT3 newPos;
		DirectX::XMStoreFloat3(&newPos, localPos);

		SetLocalPosition(entity, newPos);
		*/
	}

	void Transforms::SetPosition(Entity entity, float x, float y, float z)
	{
		DV_ASSERT(Transforms, entity);

		SetPosition(entity, { x, y, z });
	}

	void Transforms::SetLocalPosition(Entity entity, const DirectX::XMFLOAT3& pos)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Position = pos;
		//tc.MarkDirty();
	}

	void Transforms::SetLocalPosition(Entity entity, float x, float y, float z)
	{
		DV_ASSERT(Transforms, entity);

		SetLocalPosition(entity, { x, y, z });
	}

	DirectX::XMFLOAT3 Transforms::GetPosition(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();
		return tc.Position;
	}

	DirectX::XMFLOAT3 Transforms::GetLocalPosition(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		if (!HasParent(entity))
			return GetPosition(entity);

		auto parentTransform = GetTransform(GetParent(entity));
		auto parentTransformInverseMat = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&parentTransform));

		auto& tc = entity.GetComponent<TransformComponent>();
		auto positionVec = DirectX::XMLoadFloat3(&tc.Position);

		auto localPositionVec = DirectX::XMVector3TransformCoord(positionVec, parentTransformInverseMat);
		
		DirectX::XMFLOAT3 localPosition{};
		DirectX::XMStoreFloat3(&localPosition, localPositionVec);

		return localPosition;
	}

	void Transforms::SetRotationQuaternion(Entity entity, const DirectX::XMFLOAT4& rot)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Rotation = rot;
	}

	void Transforms::SetRotationByDegrees(Entity entity, const DirectX::XMFLOAT3& degrees)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();

		auto radiansX = DirectX::XMConvertToRadians(degrees.x);
		auto radiansY = DirectX::XMConvertToRadians(degrees.y);
		auto radiansZ = DirectX::XMConvertToRadians(degrees.z);

		auto rotationVec = DirectX::XMQuaternionRotationRollPitchYaw(radiansX, radiansY, radiansZ);
		rotationVec = DirectX::XMQuaternionNormalize(rotationVec);

		DirectX::XMStoreFloat4(&tc.Rotation, rotationVec);
	}

	void Transforms::SetRotationByDegrees(Entity entity, float x, float y, float z)
	{
		DV_ASSERT(Transforms, entity);

		SetRotationByDegrees(entity, { x, y, z });
	}

	void Transforms::SetLocalRotationQuaternion(Entity entity, const DirectX::XMFLOAT4& rot)
	{
		DV_ASSERT(Transforms, entity);

		if (!HasParent(entity))
		{
			SetRotationQuaternion(entity, rot);
			return;
		}

		auto parentRotation = GetParent(entity).GetComponent<TransformComponent>().Rotation;
		auto parentRotationVec = DirectX::XMLoadFloat4(&parentRotation);
		auto localRotationVec = DirectX::XMLoadFloat4(&rot);

		auto worldRotationVec = DirectX::XMQuaternionMultiply(parentRotationVec, localRotationVec);
		worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);

		DirectX::XMFLOAT4 worldRotation{};
		DirectX::XMStoreFloat4(&worldRotation, worldRotationVec);

		SetRotationQuaternion(entity, worldRotation);
	}

	void Transforms::SetLocalRotationByDegrees(Entity entity, const DirectX::XMFLOAT3& degrees)
	{
		DV_ASSERT(Transforms, entity);

		if (!HasParent(entity))
		{
			SetRotationByDegrees(entity, degrees);
			return;
		}

		auto parentRotation = GetParent(entity).GetComponent<TransformComponent>().Rotation;
		auto parentRotationVec = DirectX::XMLoadFloat4(&parentRotation);

		auto& tc = entity.GetComponent<TransformComponent>();

		auto radiansX = DirectX::XMConvertToRadians(degrees.x);
		auto radiansY = DirectX::XMConvertToRadians(degrees.y);
		auto radiansZ = DirectX::XMConvertToRadians(degrees.z);

		auto localRotationVec = DirectX::XMQuaternionRotationRollPitchYaw(radiansX, radiansY, radiansZ);

		auto worldRotationVec = DirectX::XMQuaternionMultiply(parentRotationVec, localRotationVec);
		worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);

		DirectX::XMFLOAT4 worldRotation{};
		DirectX::XMStoreFloat4(&worldRotation, worldRotationVec);

		SetRotationQuaternion(entity, worldRotation);
	}

	void Transforms::SetLocalRotationByDegrees(Entity entity, float x, float y, float z)
	{
		DV_ASSERT(Transforms, entity);

		SetLocalRotationByDegrees(entity, { x, y, z });
	}

	DirectX::XMFLOAT4 Transforms::GetRotationQuaternion(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();
		return tc.Rotation;
	}

	DirectX::XMFLOAT4 Transforms::GetLocalRotationQuaternion(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		if (!HasParent(entity))
			return GetRotationQuaternion(entity);

		auto parentRotation = GetParent(entity).GetComponent<TransformComponent>().Rotation;
		auto parentRotationInverVec = DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentRotation));

		auto worldRotation = GetRotationQuaternion(entity);
		auto worldRotationVec = DirectX::XMLoadFloat4(&worldRotation);

		auto localRotationVec = DirectX::XMQuaternionMultiply(worldRotationVec, parentRotationInverVec);
		localRotationVec = DirectX::XMQuaternionNormalize(localRotationVec);

		DirectX::XMFLOAT4 localRotation{};
		DirectX::XMStoreFloat4(&localRotation, localRotationVec);

		return localRotation;
	}

	DirectX::XMFLOAT3 Transforms::GetRotationDegrees(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		return Math::QuaternionToDegrees(GetRotationQuaternion(entity));
	}

	DirectX::XMFLOAT3 Transforms::GetLocalRotationDegrees(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		return Math::QuaternionToDegrees(GetLocalRotationQuaternion(entity));
	}

	void Transforms::SetScale(Entity entity, const DirectX::XMFLOAT3& scl)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Scale = scl;
	}

	void Transforms::SetScale(Entity entity, float x, float y, float z)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Scale = { x, y, z };
	}

	void Transforms::Translate(Entity entity, const DirectX::XMFLOAT3& move, eSpace space)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();

		if (space == eSpace::World)
		{
			tc.Position.x += move.x;
			tc.Position.y += move.y;
			tc.Position.z += move.z;
			return;
		}
		else
		{
			if (!HasParent(entity))
			{
				tc.Position.x += move.x;
				tc.Position.y += move.y;
				tc.Position.z += move.z;
				return;
			}
			else
			{
				auto parentTransform = GetTransform(GetParent(entity));
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

	void Transforms::Translate(Entity entity, float x, float y, float z, eSpace space)
	{
		DV_ASSERT(Transforms, entity);

		Translate(entity, { x, y, z }, space);
	}

	void Transforms::Rotate(Entity entity, const DirectX::XMFLOAT3& degrees, eSpace space)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();

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
			if (!HasParent(entity))
			{
				auto worldRotationVec = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&tc.Rotation), rotationVec);
				worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);
				DirectX::XMStoreFloat4(&tc.Rotation, worldRotationVec);
				return;
			}
			else
			{
				auto parentRotation = GetParent(entity).GetComponent<TransformComponent>().Rotation;
				auto parentRotationInverseVec = DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentRotation));

				auto localRotationVec = DirectX::XMQuaternionMultiply(rotationVec, parentRotationInverseVec);
				auto worldRotationVec = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&tc.Rotation), localRotationVec);
				worldRotationVec = DirectX::XMQuaternionNormalize(worldRotationVec);

				DirectX::XMStoreFloat4(&tc.Rotation, worldRotationVec);
				return;
			}
		}
	}

	void Transforms::Rotate(Entity entity, float x, float y, float z, eSpace space)
	{
		DV_ASSERT(Transforms, entity);

		Rotate(entity, { x,y,z }, space);
	}

	DirectX::XMFLOAT4X4 Transforms::GetTransform(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		if (!HasParent(entity))
			return GetLocalTransform(entity);

		auto& tc = entity.GetComponent<TransformComponent>();
		auto parentTransform = GetTransform(GetParent(entity));
		auto transformMat = DirectX::XMLoadFloat4x4(&parentTransform) * DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&tc.Scale))
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&tc.Rotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tc.Position));

		DirectX::XMFLOAT4X4 transform{};
		DirectX::XMStoreFloat4x4(&transform, transformMat);

		return transform;
	}

	DirectX::XMFLOAT4X4 Transforms::GetLocalTransform(Entity entity)
	{
		DV_ASSERT(Transforms, entity);

		auto& tc = entity.GetComponent<TransformComponent>();

		auto transformMat = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&tc.Scale))
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&tc.Rotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tc.Position));

		DirectX::XMFLOAT4X4 transform{};
		DirectX::XMStoreFloat4x4(&transform, transformMat);

		return transform;
	}

	void Transforms::SetParent(Entity child, Entity parent)
	{
		DV_ASSERT(Transforms, child);

		// 새로운 부모가 자식인 경우 확인
		if (parent && IsChildOf(child, parent))
			return;

		RemoveParent(child);

		s_Hierarchy[child].Parent = parent;
		s_Hierarchy[parent].Children.push_back(child);
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
}