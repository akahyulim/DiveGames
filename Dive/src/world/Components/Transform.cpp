#include "stdafx.h"
#include "Transform.h"
#include "world/GameObject.h"
#include "world/World.h"

using namespace DirectX;

namespace Dive
{
	Transform::Transform(GameObject* gameObject)
		: Component(gameObject)
	{
		DV_LOG(Transform, info, "생성: {}, {}", GetName(), GetInstanceID());
	}

	Transform::~Transform()
	{
		DV_LOG(Transform, info, "소멸: {}, {}", GetName(), GetInstanceID());
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		if (HasParent())
		{
			auto parentTransform = m_parent->GetTransform();
			auto parentMat = XMLoadFloat4x4(&parentTransform);
			auto localPosVec = XMLoadFloat3(&m_localPosition);
			auto worldPosVec = XMVector3Transform(localPosVec, parentMat);

			XMFLOAT3 worldPos;
			XMStoreFloat3(&worldPos, worldPosVec);

			return worldPos;
		}

		return m_localPosition;
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		if (HasParent())
		{
			auto parentTransform = m_parent->GetTransform();
			auto parentMat = XMLoadFloat4x4(&parentTransform);
			auto parentInvMat = XMMatrixInverse(nullptr, parentMat);
			auto worldPosVec = XMLoadFloat3(&position);
			auto localPosVec = XMVector3TransformCoord(worldPosVec, parentInvMat);

			XMStoreFloat3(&m_localPosition, localPosVec);
		}
		else
		{
			m_localPosition = position;
		}
	}

	DirectX::XMFLOAT4 Transform::GetRotationQuaternion() const
	{
		if (HasParent())
		{
			auto parentRotQuat = m_parent->GetRotationQuaternion();
			auto parentRotVec = XMLoadFloat4(&parentRotQuat);
			auto localRotVec = XMLoadFloat4(&m_localRotation);
			auto worldRotVec = XMQuaternionMultiply(localRotVec, parentRotVec);

			XMFLOAT4 worldRot;
			XMStoreFloat4(&worldRot, worldRotVec);

			return worldRot;
		}

		return m_localRotation;
	}

	DirectX::XMFLOAT3 Transform::GetRotationRadians() const
	{
		auto rotQuat = GetRotationQuaternion();
		auto rotVec = XMLoadFloat4(&rotQuat);
		auto rotMatrix = XMMatrixRotationQuaternion(rotVec);

		float pitch = std::atan2(rotMatrix.r[1].m128_f32[2], rotMatrix.r[2].m128_f32[2]);
		float yaw = std::atan2(-rotMatrix.r[0].m128_f32[2], std::sqrt(rotMatrix.r[0].m128_f32[0] * rotMatrix.r[0].m128_f32[0] + rotMatrix.r[0].m128_f32[1] * rotMatrix.r[0].m128_f32[1]));
		float roll = std::atan2(rotMatrix.r[0].m128_f32[1], rotMatrix.r[0].m128_f32[0]);

		return { pitch, yaw, roll };

	}

	DirectX::XMFLOAT3 Transform::GetRotationDegrees() const
	{
		auto rotationRadians = GetRotationRadians();
		return XMFLOAT3(
			XMConvertToDegrees(rotationRadians.x),
			XMConvertToDegrees(rotationRadians.y),
			XMConvertToDegrees(rotationRadians.z)
		);
	}

	void Transform::SetRotationByQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		XMVECTOR localRotVec;

		if (HasParent())
		{
			auto parentRotQuat = m_parent->GetRotationQuaternion();
			auto parentRotVec = XMLoadFloat4(&parentRotQuat);
			auto parentInvRotVec = XMQuaternionInverse(parentRotVec);
			localRotVec = XMQuaternionMultiply(parentInvRotVec, XMLoadFloat4(&quaternion));
		}
		else
		{
			localRotVec = XMLoadFloat4(&quaternion);
		}

		localRotVec = XMQuaternionNormalize(localRotVec);
		XMStoreFloat4(&m_localRotation, localRotVec);
	}

	void Transform::SetRotationByRadians(const DirectX::XMFLOAT3& radians)
	{
		auto rotVec = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radians));

		XMFLOAT4 quaternion;
		XMStoreFloat4(&quaternion, rotVec);
		SetRotationByQuaternion(quaternion);
	}

	void Transform::SetRotationByDegrees(const DirectX::XMFLOAT3& degrees)
	{
		SetRotationByRadians({
			XMConvertToRadians(degrees.x),
			XMConvertToRadians(degrees.y),
			XMConvertToRadians(degrees.z)
			});
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotationRadians() const
	{
		auto localRotQuat = GetLocalRotationQuaternion();
		auto rotVec = XMLoadFloat4(&localRotQuat);
		auto rotMatrix = XMMatrixRotationQuaternion(rotVec);

		float pitch = std::atan2(rotMatrix.r[1].m128_f32[2], rotMatrix.r[2].m128_f32[2]);
		float yaw = std::atan2(-rotMatrix.r[0].m128_f32[2], std::sqrt(rotMatrix.r[0].m128_f32[0] * rotMatrix.r[0].m128_f32[0] + rotMatrix.r[0].m128_f32[1] * rotMatrix.r[0].m128_f32[1]));
		float roll = std::atan2(rotMatrix.r[0].m128_f32[1], rotMatrix.r[0].m128_f32[0]);

		return { pitch, yaw, roll };
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotationDegrees() const
	{
		auto rotRad = GetLocalRotationRadians();
		
		return {
			XMConvertToDegrees(rotRad.x),
			XMConvertToDegrees(rotRad.y),
			XMConvertToDegrees(rotRad.z)
		};
	}

	void Transform::SetLocalRotationQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		auto localRotVec = XMLoadFloat4(&quaternion);
		localRotVec = XMQuaternionNormalize(localRotVec);
		XMStoreFloat4(&m_localRotation, localRotVec);
	}

	void Transform::SetLocalRotationByRadians(const DirectX::XMFLOAT3& radians)
	{
		auto rotVec = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radians));
		XMStoreFloat4(&m_localRotation, rotVec);
	}

	void Transform::SetLocalRotationByDegrees(const DirectX::XMFLOAT3& degrees)
	{
		SetLocalRotationByRadians({
			XMConvertToRadians(degrees.x),
			XMConvertToRadians(degrees.y),
			XMConvertToRadians(degrees.z)
			});
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		if (HasParent())
		{
			auto parentScale = m_parent->GetScale();
			auto parentScaleVec = XMLoadFloat3(&parentScale);
			auto localScaleVec = XMLoadFloat3(&m_localScale);
			auto worldScaleVec = XMVectorMultiply(localScaleVec, parentScaleVec);

			XMFLOAT3 worldScale;
			XMStoreFloat3(&worldScale, worldScaleVec);

			return worldScale;
		}
		else
		{
			return m_localScale;
		}
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		if (HasParent())
		{
			auto parentScale = m_parent->GetScale();
			auto parentScaleVec = XMLoadFloat3(&parentScale);
			auto localScaleVec = XMVectorDivide(XMLoadFloat3(&scale), parentScaleVec);

			XMStoreFloat3(&m_localScale, localScaleVec);

		}
		else
		{
			m_localScale = scale;
		}
	}
	
	DirectX::XMFLOAT4X4 Transform::GetTransform() const
	{
		if (HasParent())
		{
			auto parentTransform = m_parent->GetTransform();
			auto parentMat = XMLoadFloat4x4(&parentTransform);
			auto localMat = XMMatrixScalingFromVector(XMLoadFloat3(&m_localScale))
				* XMMatrixRotationQuaternion(XMLoadFloat4(&m_localRotation))
				* XMMatrixTranslationFromVector(XMLoadFloat3(&m_localPosition));

			XMFLOAT4X4 worldTransform;
			XMStoreFloat4x4(&worldTransform, parentMat * localMat);

			return worldTransform;
		}

		return GetLocalTransform();
	}

	DirectX::XMMATRIX Transform::GetTransformMatrix() const
	{
		auto transform = GetTransform();
		return XMLoadFloat4x4(&transform);
	}

	DirectX::XMFLOAT4X4 Transform::GetLocalTransform() const
	{
		auto localMat = XMMatrixScalingFromVector(XMLoadFloat3(&m_localScale))
			* XMMatrixRotationQuaternion(XMLoadFloat4(&m_localRotation))
			* XMMatrixTranslationFromVector(XMLoadFloat3(&m_localPosition));

		XMFLOAT4X4 localTransform;
		XMStoreFloat4x4(&localTransform, localMat);

		return localTransform;
	}

	DirectX::XMMATRIX Transform::GetLocalTransformMatrix() const
	{
		auto localTransform = GetLocalTransform();
		return XMLoadFloat4x4(&localTransform);
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
			auto translationVec = XMLoadFloat3(&move);

			if (HasParent())
			{
				auto parentRotQuat = m_parent->GetRotationQuaternion();
				auto parentRotVec = XMLoadFloat4(&parentRotQuat);
				translationVec = XMVector3Rotate(translationVec, parentRotVec);
			}

			auto newLocalPos = XMVectorAdd(XMLoadFloat3(&m_localPosition), translationVec);
			XMStoreFloat3(&m_localPosition, newLocalPos);
		}
	}

	void Transform::RotateByQuaternion(const DirectX::XMFLOAT4& quaternion, eSpace space)
	{
		XMVECTOR rotationVec = XMLoadFloat4(&quaternion);

		if (space == eSpace::World)
		{
			auto worldRotQuat = GetRotationQuaternion();
			auto currentWorldRot = XMLoadFloat4(&worldRotQuat);
			auto newWorldRot = XMQuaternionMultiply(rotationVec, currentWorldRot);

			if (HasParent())
			{
				auto parentRotQuat = m_parent->GetRotationQuaternion();
				auto parentInvRotVec = XMQuaternionInverse(XMLoadFloat4(&parentRotQuat));
				newWorldRot = XMQuaternionMultiply(parentInvRotVec, newWorldRot);
			}

			newWorldRot = XMQuaternionNormalize(newWorldRot);
			XMStoreFloat4(&m_localRotation, newWorldRot);
		}
		else
		{
			if (HasParent())
			{
				auto parentRotQuat = m_parent->GetRotationQuaternion();
				auto parentRotVec = XMLoadFloat4(&parentRotQuat);
				auto parentInvRotVec = XMQuaternionInverse(parentRotVec);
				rotationVec = XMQuaternionMultiply(parentInvRotVec, rotationVec);
			}

			rotationVec = XMQuaternionNormalize(rotationVec);
			XMStoreFloat4(&m_localRotation, rotationVec);
		}
	}

	void Transform::RotateByRadians(const DirectX::XMFLOAT3& radians, eSpace space)
	{
		auto rotVec = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radians));
		
		XMFLOAT4 quaternion;
		XMStoreFloat4(&quaternion, rotVec);
		RotateByQuaternion(quaternion, space);
	}

	void Transform::RotateByDegrees(const DirectX::XMFLOAT3& degrees, eSpace space)
	{
		RotateByRadians(
			{
				XMConvertToRadians(degrees.x),
				XMConvertToRadians(degrees.y),
				XMConvertToRadians(degrees.z)
			},
			space);
	}

	DirectX::XMFLOAT3 Transform::GetForward() const
	{
		const XMVECTOR localForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		auto worldTransform = GetTransform();
		XMMATRIX worldMatrix = XMLoadFloat4x4(&worldTransform);
		XMVECTOR worldForward = XMVector3TransformNormal(localForward, worldMatrix);
		worldForward = XMVector3Normalize(worldForward);

		XMFLOAT3 result;
		XMStoreFloat3(&result, worldForward);
		return result;
	}

	DirectX::XMFLOAT3 Transform::GetRight() const
	{
		const XMVECTOR localRightVec = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		
		auto worldTransform = GetTransform();
		XMMATRIX worldMatrix = XMLoadFloat4x4(&worldTransform);
		XMVECTOR worldForward = XMVector3TransformNormal(localRightVec, worldMatrix);
		worldForward = XMVector3Normalize(worldForward);

		XMFLOAT3 result;
		XMStoreFloat3(&result, worldForward);
		return result;
	}

	DirectX::XMFLOAT3 Transform::GetUp() const
	{
		const XMVECTOR localUpVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		auto worldTransform = GetTransform();
		XMMATRIX worldMatrix = XMLoadFloat4x4(&worldTransform);
		XMVECTOR worldForward = XMVector3TransformNormal(localUpVec, worldMatrix);
		worldForward = XMVector3Normalize(worldForward);

		XMFLOAT3 result;
		XMStoreFloat3(&result, worldForward);
		return result;
	}

	void Transform::SetParent(Transform* parent)
	{
		if (parent == m_parent || (parent != nullptr && parent->IsChildOf(this)))
			return;

		// 기존 부모의 자식 혹은 루트 컨테이너에서 제거
		if (m_parent != nullptr)
		{
			auto it = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this);
			if (it != m_parent->m_children.end())
			{
				m_parent->m_children.erase(it);
				m_parent = nullptr;
			}
		}
		else
			GetGameObject()->GetWorld()->DetachRoot(GetGameObject());

		// 새로운 부모의 자식 혹은 루트 컨테이너에 추가
		if (parent != nullptr)
			parent->m_children.push_back(this);
		else
			GetGameObject()->GetWorld()->AttachRoot(GetGameObject());

		m_parent = parent;
	}

	bool Transform::IsChildOf(Transform* parent)
	{
		assert(parent != nullptr);

		if(m_parent == nullptr)
			return false;

		if (m_parent == parent)
			return true;
		
		return m_parent->IsChildOf(parent);
	}

	Transform* Transform::GetChild(size_t index)
	{
		return m_children.size() > index ? m_children[index] : nullptr;
	}

	Transform* Transform::Find(const std::string& name)
	{
		for (auto& child : m_children)
		{
			if (child->GetGameObject()->GetName() == name)
				return child;
		}

		return nullptr;
	}

	size_t Transform::GetChildCount()
	{
		return m_children.size();
	}

	void Transform::DetachChildren()
	{
		for (auto& child : m_children)
		{
			child->m_parent = nullptr;
		}
		m_children.clear();
	}

	void Transform::RemoveChild(Transform* child)
	{
		if (!child)
			return;

		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it == m_children.end())
			return;

		m_children.erase(it);
	}

	size_t Transform::GetSiblingIndex()
	{
		assert(GetGameObject());

		if (HasParent())
		{
			const auto& sibling = m_parent->GetChildren();
			auto it = std::find(sibling.begin(), sibling.end(), this);
			
			return (it != sibling.end()) ? std::distance(sibling.begin(), it) : std::numeric_limits<size_t>::max();
		}
		else
		{
			const auto& roots = GetGameObject()->GetWorld()->m_rootGameObjects;
			auto it = std::find(roots.begin(), roots.end(), GetGameObject());

			return (it != roots.end()) ? std::distance(roots.begin(), it) : std::numeric_limits<size_t>::max();
		}
	}

	void Transform::SetSiblingIndex(size_t index)
	{
		assert(GetGameObject());

		if (HasParent())
		{
			auto& sibling = m_parent->GetChildren();
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
			auto& roots = GetGameObject()->GetWorld()->m_rootGameObjects;
			if (index >= roots.size())
				return;

			auto it = std::find(roots.begin(), roots.end(), GetGameObject());
			if (it != roots.end())
			{
				roots.erase(it);
				roots.insert(roots.begin() + index, GetGameObject());
			}
		}
	}
}
