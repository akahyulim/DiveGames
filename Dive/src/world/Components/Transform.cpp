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

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		if (HasParent())
			return XMVector3Transform(GetLocalPositionVector(), m_parent->GetTransformMatrix());

		return GetLocalPositionVector();
	}
	
	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		XMFLOAT3 result;
		XMStoreFloat3(&result, GetPositionVector());
		return result;
	}

	void Transform::SetPositionVector(const DirectX::XMVECTOR& position)
	{
		if (HasParent())
		{
			XMMATRIX parentInvMat = XMMatrixInverse(nullptr, m_parent->GetTransformMatrix());
			XMVECTOR localPosVec = XMVector3TransformCoord(position, parentInvMat);
			XMStoreFloat3(&m_localPosition, localPosVec);
		}
		else
		{
			XMStoreFloat3(&m_localPosition, position);
		}
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		SetPositionVector(XMLoadFloat3(&position));
	}

	DirectX::XMVECTOR Transform::GetRotationVector() const
	{
		if (HasParent())
			return XMQuaternionMultiply(GetLocalRotationVector(), m_parent->GetRotationVector());

		return GetLocalRotationVector();
	}

	DirectX::XMFLOAT4 Transform::GetRotation() const
	{
		XMFLOAT4 result;
		XMStoreFloat4(&result, GetRotationVector());
		return result;
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
			auto parentInvRotVec = XMQuaternionInverse(m_parent->GetRotationVector());
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
		XMStoreFloat4(&m_localRotation, XMQuaternionNormalize(quaternion));
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotationVector(XMLoadFloat4(&quaternion));
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

	DirectX::XMVECTOR Transform::GetScaleVector() const
	{
		if (HasParent())
			return XMVectorMultiply(GetLocalScaleVector(), m_parent->GetScaleVector());

		return GetLocalScaleVector();
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		XMFLOAT3 result;
		XMStoreFloat3(&result, GetScaleVector());
		return result;
	}

	void Transform::SetScaleVector(const DirectX::XMVECTOR& scale)
	{
		if (HasParent())
		{
			auto localScaleVec = XMVectorDivide(scale, m_parent->GetScaleVector());
			XMStoreFloat3(&m_localScale, localScaleVec);
		}
		else
		{
			XMStoreFloat3(&m_localScale, scale);
		}
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		SetScaleVector(XMLoadFloat3(&scale));
	}

	DirectX::XMMATRIX Transform::GetTransformMatrix() const
	{
		if (HasParent())
			return m_parent->GetTransformMatrix() * GetLocalTransformMatrix();

		return GetLocalTransformMatrix();
	}

	DirectX::XMFLOAT4X4 Transform::GetTransform() const
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, GetTransformMatrix());
		return result;
	}

	DirectX::XMMATRIX Transform::GetLocalTransformMatrix() const
	{
		return XMMatrixScalingFromVector(GetLocalScaleVector())
			* XMMatrixRotationQuaternion(GetLocalRotationVector())
			* XMMatrixTranslationFromVector(GetLocalPositionVector());
	}

	DirectX::XMFLOAT4X4 Transform::GetLocalTransform() const
	{	
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, GetLocalTransformMatrix());
		return result;
	}

	void Transform::Translate(const DirectX::XMFLOAT3& move, eSpace space)
	{
		using namespace DirectX;
		XMVECTOR moveVec = XMLoadFloat3(&move);

		if (space == eSpace::World)
		{
			// 월드 축 그대로 적용: 회전 금지
			XMVECTOR worldPos = GetPositionVector();
			XMVECTOR newWorldPos = XMVectorAdd(worldPos, moveVec);

			if (HasParent())
			{
				XMMATRIX parentInv = XMMatrixInverse(nullptr, m_parent->GetTransformMatrix());
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
		using namespace DirectX;
		XMVECTOR inputRot = XMLoadFloat4(&quaternion);

		if (space == eSpace::World)
		{
			// 월드 축 회전 누적: newWorld = input * currentWorld
			XMVECTOR currentWorld = GetRotationVector();
			XMVECTOR newWorld = XMQuaternionMultiply(inputRot, currentWorld);

			if (HasParent())
			{
				XMVECTOR parentWorld = m_parent->GetRotationVector();
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
