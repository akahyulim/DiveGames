#include "stdafx.h"
#include "Transform.h"
#include "world/GameObject.h"
#include "world/World.h"

namespace Dive
{
	Transform::Transform(GameObject* gameObject)
		: Component(gameObject)
	{
		DV_LOG(Transform, trace, "Created: {}, {}", gameObject->GetName(), gameObject->GetInstanceID());
	}

	Transform::~Transform()
	{
		//if (m_Parent)
		{
		//	auto it = std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this);
		//	if (it != m_Parent->m_Children.end())
		//		m_Parent->m_Children.erase(it);
		}
		
		DV_LOG(Transform, trace, "Destroyed: {}, {}", m_GameObject->GetName(), m_GameObject->GetInstanceID());
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		if (HasParent())
		{
			auto parentTransform = m_Parent->GetTransform();
			auto parentMat = DirectX::XMLoadFloat4x4(&parentTransform);
			auto localPosVec = DirectX::XMLoadFloat3(&m_LocalPosition);
			auto worldPosVec = DirectX::XMVector3Transform(localPosVec, parentMat);

			DirectX::XMFLOAT3 worldPos;
			DirectX::XMStoreFloat3(&worldPos, worldPosVec);

			return worldPos;
		}

		return m_LocalPosition;
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		if (HasParent())
		{
			auto parentTransform = m_Parent->GetTransform();
			auto parentMat = DirectX::XMLoadFloat4x4(&parentTransform);
			auto parentInvMat = DirectX::XMMatrixInverse(nullptr, parentMat);
			auto worldPosVec = DirectX::XMLoadFloat3(&position);
			auto localPosVec = DirectX::XMVector3TransformCoord(worldPosVec, parentInvMat);

			DirectX::XMStoreFloat3(&m_LocalPosition, localPosVec);
		}
		else
		{
			m_LocalPosition = position;
		}
	}

	DirectX::XMFLOAT4 Transform::GetRotationQuaternion() const
	{
		if (HasParent())
		{
			auto parentRotQuat = m_Parent->GetRotationQuaternion();
			auto parentRotVec = DirectX::XMLoadFloat4(&parentRotQuat);
			auto localRotVec = DirectX::XMLoadFloat4(&m_LocalRotation);
			auto worldRotVec = DirectX::XMQuaternionMultiply(localRotVec, parentRotVec);

			DirectX::XMFLOAT4 worldRot;
			DirectX::XMStoreFloat4(&worldRot, worldRotVec);

			return worldRot;
		}

		return m_LocalRotation;
	}

	DirectX::XMFLOAT3 Transform::GetRotationRadians() const
	{
		auto rotQuat = GetRotationQuaternion();
		auto rotVec = DirectX::XMLoadFloat4(&rotQuat);
		auto rotMatrix = DirectX::XMMatrixRotationQuaternion(rotVec);

		float pitch = std::atan2(rotMatrix.r[1].m128_f32[2], rotMatrix.r[2].m128_f32[2]);
		float yaw = std::atan2(-rotMatrix.r[0].m128_f32[2], std::sqrt(rotMatrix.r[0].m128_f32[0] * rotMatrix.r[0].m128_f32[0] + rotMatrix.r[0].m128_f32[1] * rotMatrix.r[0].m128_f32[1]));
		float roll = std::atan2(rotMatrix.r[0].m128_f32[1], rotMatrix.r[0].m128_f32[0]);

		return { pitch, yaw, roll };

	}

	DirectX::XMFLOAT3 Transform::GetRotationDegrees() const
	{
		auto rotationRadians = GetRotationRadians();
		return DirectX::XMFLOAT3(
			DirectX::XMConvertToDegrees(rotationRadians.x),
			DirectX::XMConvertToDegrees(rotationRadians.y),
			DirectX::XMConvertToDegrees(rotationRadians.z)
		);
	}

	void Transform::SetRotationByQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		DirectX::XMVECTOR localRotVec;

		if (HasParent())
		{
			auto parentRotQuat = m_Parent->GetRotationQuaternion();
			auto parentRotVec = DirectX::XMLoadFloat4(&parentRotQuat);
			auto parentInvRotVec = DirectX::XMQuaternionInverse(parentRotVec);
			localRotVec = DirectX::XMQuaternionMultiply(parentInvRotVec, DirectX::XMLoadFloat4(&quaternion));
		}
		else
		{
			localRotVec = DirectX::XMLoadFloat4(&quaternion);
		}

		localRotVec = DirectX::XMQuaternionNormalize(localRotVec);
		DirectX::XMStoreFloat4(&m_LocalRotation, localRotVec);
	}

	void Transform::SetRotationByRadians(const DirectX::XMFLOAT3& radians)
	{
		auto rotVec = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&radians));

		DirectX::XMFLOAT4 quaternion;
		DirectX::XMStoreFloat4(&quaternion, rotVec);
		SetRotationByQuaternion(quaternion);
	}

	void Transform::SetRotationByDegrees(const DirectX::XMFLOAT3& degrees)
	{
		SetRotationByRadians({
			DirectX::XMConvertToRadians(degrees.x),
			DirectX::XMConvertToRadians(degrees.y),
			DirectX::XMConvertToRadians(degrees.z)
			});
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotationRadians() const
	{
		auto localRotQuat = GetLocalRotationQuaternion();
		auto rotVec = DirectX::XMLoadFloat4(&localRotQuat);
		auto rotMatrix = DirectX::XMMatrixRotationQuaternion(rotVec);

		float pitch = std::atan2(rotMatrix.r[1].m128_f32[2], rotMatrix.r[2].m128_f32[2]);
		float yaw = std::atan2(-rotMatrix.r[0].m128_f32[2], std::sqrt(rotMatrix.r[0].m128_f32[0] * rotMatrix.r[0].m128_f32[0] + rotMatrix.r[0].m128_f32[1] * rotMatrix.r[0].m128_f32[1]));
		float roll = std::atan2(rotMatrix.r[0].m128_f32[1], rotMatrix.r[0].m128_f32[0]);

		return { pitch, yaw, roll };
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotationDegrees() const
	{
		auto rotRad = GetLocalRotationRadians();
		
		return {
			DirectX::XMConvertToDegrees(rotRad.x),
			DirectX::XMConvertToDegrees(rotRad.y),
			DirectX::XMConvertToDegrees(rotRad.z)
		};
	}

	void Transform::SetLocalRotationQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		auto localRotVec = DirectX::XMLoadFloat4(&quaternion);
		localRotVec = DirectX::XMQuaternionNormalize(localRotVec);
		DirectX::XMStoreFloat4(&m_LocalRotation, localRotVec);
	}

	void Transform::SetLocalRotationByRadians(const DirectX::XMFLOAT3& radians)
	{
		auto rotVec = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&radians));
		DirectX::XMStoreFloat4(&m_LocalRotation, rotVec);
	}

	void Transform::SetLocalRotationByDegrees(const DirectX::XMFLOAT3& degrees)
	{
		SetLocalRotationByRadians({
			DirectX::XMConvertToRadians(degrees.x),
			DirectX::XMConvertToRadians(degrees.y),
			DirectX::XMConvertToRadians(degrees.z)
			});
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		if (HasParent())
		{
			auto parentScale = m_Parent->GetScale();
			auto parentScaleVec = DirectX::XMLoadFloat3(&parentScale);
			auto localScaleVec = DirectX::XMLoadFloat3(&m_LocalScale);
			auto worldScaleVec = DirectX::XMVectorMultiply(localScaleVec, parentScaleVec);

			DirectX::XMFLOAT3 worldScale;
			DirectX::XMStoreFloat3(&worldScale, worldScaleVec);

			return worldScale;
		}
		else
		{
			return m_LocalScale;
		}
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		if (HasParent())
		{
			auto parentScale = m_Parent->GetScale();
			auto parentScaleVec = DirectX::XMLoadFloat3(&parentScale);
			auto localScaleVec = DirectX::XMVectorDivide(DirectX::XMLoadFloat3(&scale), parentScaleVec);

			DirectX::XMStoreFloat3(&m_LocalScale, localScaleVec);

		}
		else
		{
			m_LocalScale = scale;
		}
	}
	
	DirectX::XMFLOAT4X4 Transform::GetTransform() const
	{
		if (HasParent())
		{
			auto parentTransform = m_Parent->GetTransform();
			auto parentMat = DirectX::XMLoadFloat4x4(&parentTransform);
			auto localMat = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_LocalScale))
				* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_LocalRotation))
				* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_LocalPosition));

			DirectX::XMFLOAT4X4 worldTransform;
			DirectX::XMStoreFloat4x4(&worldTransform, parentMat * localMat);

			return worldTransform;
		}

		return GetLocalTransform();
	}

	DirectX::XMFLOAT4X4 Transform::GetLocalTransform() const
	{
		auto localMat = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_LocalScale))
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_LocalRotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_LocalPosition));

		DirectX::XMFLOAT4X4 localTransform;
		DirectX::XMStoreFloat4x4(&localTransform, localMat);

		return localTransform;
	}

	void Transform::Translate(const DirectX::XMFLOAT3& move, eSpace space)
	{
		if (space == eSpace::World)
		{
			auto worldPosition = GetPosition();
			SetPosition({ worldPosition.x + move.x, worldPosition.y + move.y, worldPosition.z + move.z });
		}
		else
		{
			auto translationVec = DirectX::XMLoadFloat3(&move);

			if (HasParent())
			{
				auto parentRotQuat = m_Parent->GetRotationQuaternion();
				auto parentRotVec = DirectX::XMLoadFloat4(&parentRotQuat);
				translationVec = DirectX::XMVector3Rotate(translationVec, parentRotVec);
			}

			auto newLocalPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_LocalPosition), translationVec);
			DirectX::XMStoreFloat3(&m_LocalPosition, newLocalPos);
		}
	}

	void Transform::RotateByQuaternion(const DirectX::XMFLOAT4& quaternion, eSpace space)
	{
		DirectX::XMVECTOR rotationVec = DirectX::XMLoadFloat4(&quaternion);

		if (space == eSpace::World)
		{
			auto worldRotQuat = GetRotationQuaternion();
			auto currentWorldRot = DirectX::XMLoadFloat4(&worldRotQuat);
			auto newWorldRot = DirectX::XMQuaternionMultiply(rotationVec, currentWorldRot);

			if (HasParent())
			{
				auto parentRotQuat = m_Parent->GetRotationQuaternion();
				auto parentInvRotVec = DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentRotQuat));
				newWorldRot = DirectX::XMQuaternionMultiply(parentInvRotVec, newWorldRot);
			}

			newWorldRot = DirectX::XMQuaternionNormalize(newWorldRot);
			DirectX::XMStoreFloat4(&m_LocalRotation, newWorldRot);
		}
		else
		{
			if (HasParent())
			{
				auto parentRotQuat = m_Parent->GetRotationQuaternion();
				auto parentRotVec = DirectX::XMLoadFloat4(&parentRotQuat);
				auto parentInvRotVec = DirectX::XMQuaternionInverse(parentRotVec);
				rotationVec = DirectX::XMQuaternionMultiply(parentInvRotVec, rotationVec);
			}

			rotationVec = DirectX::XMQuaternionNormalize(rotationVec);
			DirectX::XMStoreFloat4(&m_LocalRotation, rotationVec);
		}
	}

	void Transform::RotateByRadians(const DirectX::XMFLOAT3& radians, eSpace space)
	{
		auto rotVec = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&radians));
		
		DirectX::XMFLOAT4 quaternion;
		DirectX::XMStoreFloat4(&quaternion, rotVec);
		RotateByQuaternion(quaternion, space);
	}

	void Transform::RotateByDegrees(const DirectX::XMFLOAT3& degrees, eSpace space)
	{
		RotateByRadians(
			{
				DirectX::XMConvertToRadians(degrees.x),
				DirectX::XMConvertToRadians(degrees.y),
				DirectX::XMConvertToRadians(degrees.z)
			},
			space);
	}

	void Transform::SetParent(Transform* parent)
	{
		assert(m_GameObject);

		if (parent == m_Parent || (parent != nullptr && parent->IsChildOf(this)))
			return;

		// 기존 부모의 자식 혹은 루트 컨테이너에서 제거
		if (m_Parent != nullptr)
		{
			auto it = std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this);
			if (it != m_Parent->m_Children.end())
			{
				m_Parent->m_Children.erase(it);
				m_Parent = nullptr;
			}
		}
		else
			m_GameObject->GetWorld()->DetachRoot(m_GameObject);

		// 새로운 부모의 자식 혹은 루트 컨테이너에 추가
		if (parent != nullptr)
			parent->m_Children.push_back(this);
		else
			m_GameObject->GetWorld()->AttachRoot(m_GameObject);

		m_Parent = parent;
	}

	bool Transform::IsChildOf(Transform* parent)
	{
		assert(parent != nullptr);

		if(m_Parent == nullptr)
			return false;

		if (m_Parent == parent)
			return true;
		
		return m_Parent->IsChildOf(parent);
	}

	Transform* Transform::GetChild(size_t index)
	{
		return m_Children.size() > index ? m_Children[index] : nullptr;
	}

	Transform* Transform::Find(const std::string& name)
	{
		for (auto child : m_Children)
		{
			if (child->GetGameObject()->GetName() == name)
				return child;
		}

		return nullptr;
	}

	size_t Transform::GetChildCount()
	{
		return m_Children.size();
	}

	void Transform::DetachChildren()
	{
		for (auto child : m_Children)
		{
			child->m_Parent = nullptr;
		}
		m_Children.clear();
	}

	void Transform::RemoveChild(Transform* child)
	{
		if (!child)
			return;

		auto it = std::find(m_Children.begin(), m_Children.end(), child);
		if (it == m_Children.end())
			return;

		m_Children.erase(it);
	}

	size_t Transform::GetSiblingIndex()
	{
		assert(m_GameObject);

		if (HasParent())
		{
			const auto& sibling = m_Parent->GetChildren();
			auto it = std::find(sibling.begin(), sibling.end(), this);
			
			return (it != sibling.end()) ? std::distance(sibling.begin(), it) : std::numeric_limits<size_t>::max();
		}
		else
		{
			const auto& roots = m_GameObject->GetWorld()->m_RootGameObjects;
			auto it = std::find(roots.begin(), roots.end(), m_GameObject->GetSharedPtr());

			return (it != roots.end()) ? std::distance(roots.begin(), it) : std::numeric_limits<size_t>::max();
		}
	}

	void Transform::SetSiblingIndex(size_t index)
	{
		assert(m_GameObject);

		if (HasParent())
		{
			auto& sibling = m_Parent->GetChildren();
			if (index >= sibling.size())
				return;

			auto it = std::find(sibling.begin(), sibling.end(), this);
			if (it != sibling.end())
			{
				sibling.erase(it);
				sibling.insert(sibling.begin() + index, this);
			}
		}
		else
		{
			auto& roots = m_GameObject->GetWorld()->m_RootGameObjects;
			if (index >= roots.size())
				return;

			auto it = std::find(roots.begin(), roots.end(), m_GameObject->GetSharedPtr());
			if (it != roots.end())
			{
				roots.erase(it);
				roots.insert(roots.begin() + index, m_GameObject->GetSharedPtr());
			}
		}
	}
}