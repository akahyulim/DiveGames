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
		XMStoreFloat4x4(&m_LocalTransform, XMMatrixIdentity());

		DV_LOG(Transform, info, "생성: {}, {}", GetName(), GetInstanceID());
	}

	Transform::~Transform()
	{
		DV_LOG(Transform, info, "소멸: {}, {}", GetName(), GetInstanceID());
	}

	void Transform::Update()
	{
		m_WasUpdated = m_HasChanged;

		if (m_HasChanged)
		{
			CalculateLocalTransform();
			m_HasChanged = false;
		}
	}

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		if (HasParent())
			return XMVector3Transform(GetLocalPositionVector(), m_Parent->GetTransformMatrix());

		return GetLocalPositionVector();
	}
	
	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		XMFLOAT3 position;
		XMStoreFloat3(&position, GetPositionVector());
		return position;
	}

	void Transform::SetPositionVector(const DirectX::XMVECTOR& position)
	{
		XMVECTOR worldPosition = position;
		XMVECTOR localPosition = worldPosition;

		if (HasParent())
		{
			XMMATRIX parentInvMat = XMMatrixInverse(nullptr, m_Parent->GetTransformMatrix());
			localPosition = XMVector3TransformCoord(worldPosition, parentInvMat);
		}

		SetLocalPositionVector(localPosition);
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		SetPositionVector(XMLoadFloat3(&position));
	}

	void Transform::SetLocalPositionVector(const DirectX::XMVECTOR& position)
	{
		XMFLOAT3 localPosition;
		XMStoreFloat3(&localPosition, position);
		SetLocalPosition(localPosition);
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		if (XMVector3NearEqual(XMLoadFloat3(&m_LocalPosition), XMLoadFloat3(&position), XMVectorSet(1e-5f, 1e-5f, 1e-5f, 0.0f)))
			return;

		m_LocalPosition = position;
		m_HasChanged = true;
	}

	DirectX::XMVECTOR Transform::GetRotationVector() const
	{
		if (HasParent())
			return XMQuaternionMultiply(GetLocalRotationVector(), m_Parent->GetRotationVector());

		return GetLocalRotationVector();
	}

	DirectX::XMFLOAT4 Transform::GetRotation() const
	{
		XMFLOAT4 rotation;
		XMStoreFloat4(&rotation, GetRotationVector());
		return rotation;
	}

	DirectX::XMFLOAT3 Transform::GetRotationRadians() const
	{
		auto rotMatrix = XMMatrixRotationQuaternion(GetRotationVector());

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

	void Transform::SetRotationVector(const DirectX::XMVECTOR& quaternion)
	{
		if (HasParent())
		{
			auto parentInvRotVec = XMQuaternionInverse(m_Parent->GetRotationVector());
			SetLocalRotationVector(XMQuaternionMultiply(parentInvRotVec, quaternion));
		}
		else
		{
			SetLocalRotationVector(quaternion);
		}
	}

	void Transform::SetRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotationVector(XMLoadFloat4(&quaternion));
	}

	void Transform::SetRotationByRadians(const DirectX::XMFLOAT3& radians)
	{
		SetRotationVector(XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radians)));
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
		auto rotMatrix = XMMatrixRotationQuaternion(GetLocalRotationVector());

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

	void Transform::SetLocalRotationVector(const DirectX::XMVECTOR& quaternion)
	{
		XMVECTOR current = XMLoadFloat4(&m_LocalRotation);;
		XMVECTOR normalized = XMQuaternionNormalize(quaternion);
		if (XMVector4NearEqual(current, normalized, XMVectorSet(1e-5f, 1e-5f, 1e-5f, 1e-5f)))
			return;

		XMStoreFloat4(&m_LocalRotation, normalized);
		m_HasChanged = true;
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotationVector(XMLoadFloat4(&quaternion));
	}

	void Transform::SetLocalRotationByRadians(const DirectX::XMFLOAT3& radians)
	{
		XMVECTOR localRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radians));
		SetLocalRotationVector(localRotation);
	}

	void Transform::SetLocalRotationByDegrees(const DirectX::XMFLOAT3& degrees)
	{
		SetLocalRotationByRadians({
			XMConvertToRadians(degrees.x),
			XMConvertToRadians(degrees.y),
			XMConvertToRadians(degrees.z)
			});
	}

	DirectX::XMVECTOR Transform::GetScaleVector() const
	{
		if (HasParent())
			return XMVectorMultiply(GetLocalScaleVector(), m_Parent->GetScaleVector());

		return GetLocalScaleVector();
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, GetScaleVector());
		return scale;
	}

	void Transform::SetScaleVector(const DirectX::XMVECTOR& scale)
	{
		XMVECTOR worldScale = scale;
		XMVECTOR localScale = worldScale;

		if (HasParent())
		{
			XMVECTOR parentScale = m_Parent->GetScaleVector();
			localScale = XMVectorDivide(worldScale, parentScale);
		}

		SetLocalScaleVector(localScale);
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		SetScaleVector(XMLoadFloat3(&scale));
	}

	void Transform::SetLocalScaleVector(const DirectX::XMVECTOR& scale)
	{
		XMFLOAT3 localScale;
		XMStoreFloat3(&localScale, scale);
		SetLocalScale(localScale);
	}

	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		if (XMVector3NearEqual(XMLoadFloat3(&m_LocalScale), XMLoadFloat3(&scale), XMVectorSet(1e-5f, 1e-5f, 1e-5f, 0.0f)))
			return;

		m_LocalScale = scale;
		m_HasChanged = true;
	}

	void Transform::Translate(const DirectX::XMFLOAT3& move, eSpace space)
	{
		XMVECTOR moveVec = XMLoadFloat3(&move);

		if (space == eSpace::World)
		{
			// 월드 축 그대로 적용: 회전 금지
			XMVECTOR worldPos = GetPositionVector();
			XMVECTOR newWorldPos = XMVectorAdd(worldPos, moveVec);

			if (HasParent())
			{
				XMMATRIX parentInv = XMMatrixInverse(nullptr, m_Parent->GetTransformMatrix());
				XMVECTOR localPos = XMVector3TransformCoord(newWorldPos, parentInv);
				SetLocalPositionVector(localPos);
			}
			else
			{
				SetLocalPositionVector(newWorldPos);
			}
		}
		else // Local
		{
			// 자기 회전 기준으로 이동 방향 회전
			XMVECTOR selfRot = GetRotationVector();
			XMVECTOR rotatedMove = XMVector3Rotate(moveVec, selfRot);

			// 로컬 위치에 더함
			XMVECTOR localPos = GetLocalPositionVector();
			SetLocalPositionVector(XMVectorAdd(localPos, rotatedMove));
		}
	}

	void Transform::Rotate(const DirectX::XMFLOAT4& quaternion, eSpace space)
	{
		XMVECTOR inputRot = XMLoadFloat4(&quaternion);

		if (space == eSpace::World)
		{
			// 월드 축 회전 누적: newWorld = input * currentWorld
			XMVECTOR currentWorld = GetRotationVector();
			XMVECTOR newWorld = XMQuaternionMultiply(inputRot, currentWorld);

			if (HasParent())
			{
				XMVECTOR parentWorld = m_Parent->GetRotationVector();
				XMVECTOR parentInv = XMQuaternionInverse(parentWorld);
				XMVECTOR newLocal = XMQuaternionMultiply(parentInv, newWorld);
				SetLocalRotationVector(newLocal);
			}
			else
			{
				SetLocalRotationVector(newWorld);
			}
		}
		else // Local
		{
			// 로컬 축 회전 누적: newLocal = input * currentLocal
			XMVECTOR currentLocal = GetLocalRotationVector();
			XMVECTOR newLocal = XMQuaternionMultiply(inputRot, currentLocal);
			SetLocalRotationVector(newLocal);
		}
	}

	void Transform::RotateByRadians(const DirectX::XMFLOAT3& radians, eSpace space)
	{
		auto rotVec = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radians));
		
		XMFLOAT4 quaternion;
		XMStoreFloat4(&quaternion, rotVec);
		Rotate(quaternion, space);
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

	void Transform::CalculateLocalTransform()
	{
		XMMATRIX localTransformMatrix = XMMatrixScalingFromVector(GetLocalScaleVector())
			* XMMatrixRotationQuaternion(GetLocalRotationVector())
			* XMMatrixTranslationFromVector(GetLocalPositionVector());

		XMStoreFloat4x4(&m_LocalTransform, localTransformMatrix);
	}

	DirectX::XMMATRIX Transform::GetTransformMatrix() const
	{
		XMMATRIX localTransform = GetLocalTransformMatrix();
		XMMATRIX worldTransform = localTransform;

		if (HasParent())
		{
			XMMATRIX parentMatrix = m_Parent->GetTransformMatrix();
			worldTransform = parentMatrix * localTransform;
		}

		return worldTransform;
	}

	DirectX::XMFLOAT4X4 Transform::GetTransform() const
	{
		XMFLOAT4X4 transform;
		XMStoreFloat4x4(&transform, GetTransformMatrix());
		return transform;
	}

	DirectX::XMVECTOR Transform::GetLocalForward() const
	{
		XMVECTOR localForward = XMVector3TransformNormal(GetForward(), GetTransformMatrix());
		return XMVector3Normalize(localForward);
	}

	DirectX::XMVECTOR Transform::GetLocalBackward() const
	{
		XMVECTOR localBackward = XMVector3TransformNormal(GetBackward(), GetTransformMatrix());
		return XMVector3Normalize(localBackward);
	}

	DirectX::XMVECTOR Transform::GetLocalRight() const
	{
		XMVECTOR localRight = XMVector3TransformNormal(GetRight(), GetTransformMatrix());
		return XMVector3Normalize(localRight);
}

	DirectX::XMVECTOR Transform::GetLocalLeft() const
	{
		XMVECTOR localLeft = XMVector3TransformNormal(GetLeft(), GetTransformMatrix());
		return XMVector3Normalize(localLeft);
	}

	DirectX::XMVECTOR Transform::GetLocalUp() const
	{
		XMVECTOR localUp = XMVector3TransformNormal(GetUp(), GetTransformMatrix());
		return XMVector3Normalize(localUp);
	}

	DirectX::XMVECTOR Transform::GetLocalDown() const
	{
		XMVECTOR localDown = XMVector3TransformNormal(GetDown(), GetTransformMatrix());
		return XMVector3Normalize(localDown);
	}

	void Transform::SetParent(Transform* parent)
	{
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
			GetGameObject()->GetWorld()->DetachRoot(GetGameObject());

		// 새로운 부모의 자식 혹은 루트 컨테이너에 추가
		if (parent != nullptr)
			parent->m_Children.push_back(this);
		else
			GetGameObject()->GetWorld()->AttachRoot(GetGameObject());

		m_Parent = parent;
		m_HasChanged = true;
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
		for (auto& child : m_Children)
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
		for (auto& child : m_Children)
		{
			child->m_Parent = nullptr;
			child->m_HasChanged = true;
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
		assert(GetGameObject());

		if (HasParent())
		{
			const auto& sibling = m_Parent->GetChildren();
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
