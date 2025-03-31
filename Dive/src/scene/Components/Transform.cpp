#include "stdafx.h"
#include "Transform.h"
#include "../GameObject.h"
#include "../Scene.h"
#include "core/CoreDefs.h"
#include "core/EventDispatcher.h"
#include "math/Math.h"

using namespace DirectX;

namespace Dive
{
	Transform::Transform(GameObject* pGameObject)
		: Component(pGameObject)
		, m_pParent(nullptr)
		, m_ParentID(0)
	{
		m_LocalPosition = { 0.0f, 0.0f, 0.0f };
		m_LocalRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_LocalScale = { 1.0f, 1.0f, 1.0f };

		DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_LocalTransform, DirectX::XMMatrixIdentity());
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		return { m_Transform._41, m_Transform._42, m_Transform._43 };
	}

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		return DirectX::XMVectorSet(m_Transform._41, m_Transform._42, m_Transform._43, 1.0f);
	}

	void Transform::SetPosition(float posX, float posY, float posZ)
	{
		SetPosition(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		SetPosition(DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f));
	}

	void Transform::SetPosition(const DirectX::XMVECTOR& position)
	{
		DirectX::XMVECTOR localPosition = position;

		if (m_pParent)
		{
			const auto parentSceneTransformInverse = DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix());
			localPosition = DirectX::XMVector3Transform(position, parentSceneTransformInverse);
		}

		SetLocalPosition(DirectX::XMVectorGetX(localPosition), DirectX::XMVectorGetY(localPosition), DirectX::XMVectorGetZ(localPosition));
	}

	DirectX::XMVECTOR Transform::GetLocalPositionVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalPosition);
	}

	void Transform::SetLocalPosition(float posX, float posY, float posZ)
	{
		SetLocalPosition(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		SetLocalPosition(DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f));
	}

	void Transform::SetLocalPosition(const DirectX::XMVECTOR& position)
	{
		if (!DirectX::XMVector3Equal(GetLocalPositionVector(), position))
		{
			DirectX::XMStoreFloat3(&m_LocalPosition, position);
			updateTransform();

			DV_FIRE_EVENT(eEventType::SceneModified);
		}
	}

	DirectX::XMFLOAT3 Transform::GetRotationDegrees() const
	{
		return Math::QuaternionToEuler(GetRotationQuaternionVector());
	}

	DirectX::XMFLOAT4 Transform::GetRotationQuaternion() const
	{
		const auto rotation = GetRotationQuaternionVector();
		return { DirectX::XMVectorGetX(rotation),DirectX::XMVectorGetY(rotation), DirectX::XMVectorGetZ(rotation), DirectX::XMVectorGetW(rotation) };
	}

	DirectX::XMVECTOR Transform::GetRotationQuaternionVector() const
	{
		DirectX::XMVECTOR position, rotation, scale;
		DirectX::XMMatrixDecompose(&scale, &rotation, &position, DirectX::XMLoadFloat4x4(&m_Transform));

		return rotation;
	}


	void Transform::SetRotation(float degreeX, float degreeY, float degreeZ)
	{
		const float roll = DirectX::XMConvertToRadians(degreeX);
		const float pitch = DirectX::XMConvertToRadians(degreeY);
		const float yaw = DirectX::XMConvertToRadians(degreeZ);

		const auto worldRotation = DirectX::XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);

		SetRotation(worldRotation);
	}

	void Transform::SetRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotation(DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w));
	}

	void Transform::SetRotation(const DirectX::XMVECTOR& quaternion)
	{
		DirectX::XMVECTOR localRotation = quaternion;

		if (m_pParent)
		{
			const auto parentSceneRotationInverse = DirectX::XMQuaternionInverse(m_pParent->GetRotationQuaternionVector());
			localRotation = DirectX::XMQuaternionMultiply(parentSceneRotationInverse, quaternion);
		}

		SetLocalRotation(localRotation);
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotationDegrees() const
	{
		return Math::QuaternionToEuler(GetLocalRotationQuaternionVector());
	}

	DirectX::XMVECTOR Transform::GetLocalRotationQuaternionVector() const
	{
		return DirectX::XMLoadFloat4(&m_LocalRotation);
	}

	void Transform::SetLocalRotation(float degreeX, float degreeY, float degreeZ)
	{
		float roll = DirectX::XMConvertToRadians(degreeX);
		float pitch = DirectX::XMConvertToRadians(degreeY);
		float yaw = DirectX::XMConvertToRadians(degreeZ);

		auto localRotation = DirectX::XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);

		SetLocalRotation(localRotation);
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotation(DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w));
	}

	void Transform::SetLocalRotation(const DirectX::XMVECTOR& quaternion)
	{
		if (!DirectX::XMVector4Equal(GetRotationQuaternionVector(), quaternion))
		{
			DirectX::XMStoreFloat4(&m_LocalRotation, quaternion);
			updateTransform();

			DV_FIRE_EVENT(eEventType::SceneModified);
		}
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		const auto scale = GetScaleVector();
		return { DirectX::XMVectorGetX(scale), DirectX::XMVectorGetY(scale), DirectX::XMVectorGetZ(scale) };
	}

	DirectX::XMVECTOR Transform::GetScaleVector() const
	{
		DirectX::XMVECTOR position, rotation, scale;
		DirectX::XMMatrixDecompose(&scale, &rotation, &position, DirectX::XMLoadFloat4x4(&m_Transform));

		return scale;
	}

	void Transform::SetScale(float sclX, float sclY, float sclZ)
	{
		SetScale(DirectX::XMVectorSet(sclX, sclY, sclZ, 1.0f));
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		SetScale(DirectX::XMVectorSet(scale.x, scale.y, scale.z, 1.0f));
	}

	void Transform::SetScale(const DirectX::XMVECTOR& scale)
	{
		DirectX::XMVECTOR localScale = scale;

		if (m_pParent)
		{
			const auto parentScale = m_pParent->GetScaleVector();
			localScale = DirectX::XMVectorDivide(scale, parentScale);
		}

		SetLocalScale(localScale);
	}

	DirectX::XMVECTOR Transform::GetLocalScaleVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalScale);
	}

	void Transform::SetLocalScale(float sclX, float sclY, float sclZ)
	{
		SetLocalScale(DirectX::XMVectorSet(sclX, sclY, sclZ, 1.0f));
	}

	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		SetLocalScale(DirectX::XMVectorSet(scale.x, scale.y, scale.z, 1.0f));
	}

	void Transform::SetLocalScale(const DirectX::XMVECTOR& scale)
	{
		if (!DirectX::XMVector3Equal(GetScaleVector(), scale))
		{
			DirectX::XMStoreFloat3(&m_LocalScale, scale);
			updateTransform();

			DV_FIRE_EVENT(eEventType::SceneModified);
		}
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		if (relativeTo == eSpace::Scene)
		{
			auto position = GetPosition();
			SetPosition(position.x + x, position.y + y, position.z + z);
		}
		else
		{
			auto localRotationQauternion = GetLocalRotationQuaternion();
			auto localRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&localRotationQauternion));
			auto localTranslate = DirectX::XMVector3Transform(DirectX::XMVectorSet(x, y, z, 1.0f), localRotationMatrix);

			auto localPosition = GetLocalPosition();
			SetLocalPosition(localPosition.x + DirectX::XMVectorGetX(localTranslate),
				localPosition.y + DirectX::XMVectorGetY(localTranslate),
				localPosition.z + DirectX::XMVectorGetZ(localTranslate));
		}
	}

	// 이건 급작스레 추가한 것인데
	// 아래의 원본에 맞춰 수정해야 한다.
	void Transform::Rotate(const DirectX::XMVECTOR& quaternion, eSpace relativeTo)
	{
		auto currentSceneRotationQuaternion = DirectX::XMLoadFloat4(&m_LocalRotation);

		if (relativeTo == eSpace::Scene)
		{
			auto newSceneRotationQuaternion = DirectX::XMQuaternionMultiply(currentSceneRotationQuaternion, quaternion);
			DirectX::XMStoreFloat4(&m_LocalRotation, newSceneRotationQuaternion);
		}
		else
		{
			auto newSceneRotationQuaternion = DirectX::XMQuaternionMultiply(quaternion, currentSceneRotationQuaternion);
			DirectX::XMStoreFloat4(&m_LocalRotation, newSceneRotationQuaternion);
		}

		updateTransform();
	}

	// Translate처럼 SetLocalRoatation()을 통해 updateTransform()을 수행토록 변경했다.
	// 특별한 처이점은 없다.
	void Transform::Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo)
	{
		float roll = DirectX::XMConvertToRadians(degreeX);
		float pitch = DirectX::XMConvertToRadians(degreeY);
		float yaw = DirectX::XMConvertToRadians(degreeZ);

		auto rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);
		auto currentSceneRotationQuaternion = DirectX::XMLoadFloat4(&m_LocalRotation);

		if (relativeTo == eSpace::Scene)
		{
			auto newSceneRotationQuaternion = DirectX::XMQuaternionMultiply(currentSceneRotationQuaternion, rotationQuaternion);
			SetLocalRotation(newSceneRotationQuaternion);
			//DirectX::XMStoreFloat4(&m_LocalRotation, newSceneRotationQuaternion);
		}
		else
		{
			auto newSceneRotationQuaternion = DirectX::XMQuaternionMultiply(rotationQuaternion, currentSceneRotationQuaternion);
			SetLocalRotation(newSceneRotationQuaternion);
			//DirectX::XMStoreFloat4(&m_LocalRotation, newSceneRotationQuaternion);
		}

		//updateTransform();
	}

	void Transform::LookAt(float x, float y, float z, float upX, float upY, float upZ)
	{
		LookAt(DirectX::XMVectorSet(x, y, z, 1.0f), DirectX::XMVectorSet(upX, upY, upZ, 1.0f));
	}

	void Transform::LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
	{
		LookAt(DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f), DirectX::XMVectorSet(up.x, up.y, up.z, 1.0f));
	}

	void Transform::LookAt(const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& up)
	{
		if (DirectX::XMVector3Equal(target, GetPositionVector()))
		{
			DV_LOG(GameObject, warn, "현재 위치를 바라볼 수 없습니다.");
			return;
		}

		auto forwardVector = DirectX::XMVectorSubtract(target, GetPositionVector());
		forwardVector = DirectX::XMVector3Normalize(forwardVector);

		if (DirectX::XMVector3NearEqual(DirectX::XMVector3Dot(forwardVector, up), DirectX::XMVector3Length(up), DirectX::XMVectorReplicate(0.0001f)))
		{
			DV_LOG(GameObject, warn, "바라보는 대상과 상향 벡터가 평행하여 적용이 불가능합니다.");
			return;
		}

		auto rightVector = DirectX::XMVector3Cross(up, forwardVector);
		rightVector = DirectX::XMVector3Normalize(rightVector);

		auto upVector = DirectX::XMVector3Cross(forwardVector, rightVector);
		upVector = DirectX::XMVector3Normalize(upVector);

		auto transform = DirectX::XMMATRIX(
			DirectX::XMVectorScale(rightVector, GetScale().x),
			DirectX::XMVectorScale(upVector, GetScale().y),
			DirectX::XMVectorScale(forwardVector, GetScale().z),
			GetPositionVector());

		SetMatrix(transform);
	}

	void Transform::LookAt(const Transform* pTarget, const DirectX::XMVECTOR& up)
	{
		if (!pTarget)
		{
			DV_LOG(GameObject, warn, "바라보는 대상 게임오브젝트가 존재하지 않습니다.");
			return;
		}

		LookAt(pTarget->GetPositionVector(), up);
	}

	void Transform::SetTransform(const DirectX::XMFLOAT4X4& world)
	{
		auto worldMatrix = DirectX::XMLoadFloat4x4(&world);
		SetMatrix(worldMatrix);
	}

	void Transform::SetMatrix(const DirectX::XMMATRIX& world)
	{
		DirectX::XMMATRIX localMatrix = world;

		if (m_pParent)
		{
			const auto parentSceneMatrixInverse = DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix());
			localMatrix = DirectX::XMMatrixMultiply(parentSceneMatrixInverse, world);
		}

		SetLocalMatrix(localMatrix);
	}

	void Transform::SetLocalTransform(const DirectX::XMFLOAT4X4& local)
	{
		auto localMatrix = DirectX::XMLoadFloat4x4(&local);
		SetLocalMatrix(localMatrix);
	}

	void Transform::SetLocalMatrix(const DirectX::XMMATRIX& local)
	{
		DirectX::XMVECTOR position, rotation, scale;
		DirectX::XMMatrixDecompose(&scale, &rotation, &position, local);

		DirectX::XMStoreFloat3(&m_LocalPosition, position);
		DirectX::XMStoreFloat4(&m_LocalRotation, rotation);
		DirectX::XMStoreFloat3(&m_LocalScale, scale);

		updateTransform();
	}

	DirectX::XMFLOAT3 Transform::GetForward() const
	{
		DirectX::XMFLOAT3 forward;
		DirectX::XMStoreFloat3(&forward, GetForwardVector());

		return forward;
	}

	DirectX::XMVECTOR Transform::GetForwardVector() const
	{
		auto forward = DirectX::XMVectorSet(m_Transform._31, m_Transform._32, m_Transform._33, 0.0f);
		return DirectX::XMVector3Normalize(forward);
	}

	DirectX::XMFLOAT3 Transform::GetUpward() const
	{
		DirectX::XMFLOAT3 upward;
		DirectX::XMStoreFloat3(&upward, GetUpwardVector());

		return upward;
	}

	DirectX::XMVECTOR Transform::GetUpwardVector() const
	{
		auto upward = DirectX::XMVectorSet(m_Transform._21, m_Transform._22, m_Transform._23, 0.0f);
		return DirectX::XMVector3Normalize(upward);
	}

	DirectX::XMFLOAT3 Transform::GetRightward() const
	{
		DirectX::XMFLOAT3 rightward;
		DirectX::XMStoreFloat3(&rightward, GetRightwardVector());

		return rightward;
	}

	DirectX::XMVECTOR Transform::GetRightwardVector() const
	{
		auto rightward = DirectX::XMVectorSet(m_Transform._11, m_Transform._12, m_Transform._13, 0.0f);
		return DirectX::XMVector3Normalize(rightward);
	}

	void Transform::SetParent(Transform* pParent)
	{
		if (pParent == this)
			return;

		DV_FIRE_EVENT(eEventType::SceneModified);

		if (pParent)
		{
			if (m_pParent)
			{
				if (m_pParent == pParent)
					return;

				auto it = m_pParent->m_Children.begin();
				for (it; it != m_pParent->m_Children.end(); ++it)
				{
					if ((*it) == this)
					{
						m_pParent->m_Children.erase(it);
						break;
					}
				}
			}

			m_pParent = pParent;
			m_ParentID = pParent->GetGameObject()->GetID();
			pParent->m_Children.emplace_back(this);

			// 로컬 값들을 계산
			auto parentMatrix = DirectX::XMMatrixScalingFromVector(m_pParent->GetScaleVector()) *
				DirectX::XMMatrixRotationQuaternion(m_pParent->GetRotationQuaternionVector());

			auto localPos = DirectX::XMVectorSubtract(GetPositionVector(), m_pParent->GetPositionVector());
			localPos = DirectX::XMVector3Transform(localPos, DirectX::XMMatrixInverse(nullptr, parentMatrix));
			DirectX::XMStoreFloat3(&m_LocalPosition, localPos);

			auto localRot = DirectX::XMQuaternionMultiply(GetRotationQuaternionVector(), DirectX::XMQuaternionInverse(m_pParent->GetRotationQuaternionVector()));
			DirectX::XMStoreFloat4(&m_LocalRotation, localRot);

			auto parentScale = m_pParent->GetScaleVector();
			auto worldScale = GetScaleVector();
			auto localScale = DirectX::XMVectorDivide(worldScale, parentScale);
			DirectX::XMStoreFloat3(&m_LocalScale, localScale);

			updateTransform();

			return;
		}
		else
		{
			m_ParentID = 0;
			if (m_pParent)
			{
				auto it = m_pParent->m_Children.begin();
				for (it; it != m_pParent->m_Children.end(); ++it)
				{
					if ((*it) == this)
					{
						m_pParent->m_Children.erase(it);
						m_pParent = nullptr;

						m_LocalPosition = GetPosition();
						m_LocalRotation = GetRotationQuaternion();
						m_LocalScale = GetScale();

						updateTransform();

						return;
					}
				}
			}
		}
	}

	Transform* Transform::GetRoot()
	{
		if (!m_pParent)
			return this;

		return m_pParent->GetRoot();
	}

	Transform* Transform::GetChild(uint32_t index) const
	{
		if (m_Children.empty() || index > (uint32_t)m_Children.size())
			return nullptr;

		return m_Children[index];
	}

	bool Transform::IsChildOf(Transform* pParent) const
	{
		if (pParent == this)
			return true;

		if (!m_pParent)
			return false;
		else
			return m_pParent->IsChildOf(pParent);
	}

	void Transform::DetachChildren()
	{
		for (auto* pChild : m_Children)
		{
			pChild->SetParent(nullptr);
		}
		m_Children.clear();
		m_Children.shrink_to_fit();
	}

	void Transform::updateTransform()
	{
		const auto localMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_LocalScale)) *
			DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_LocalRotation)) *
			DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_LocalPosition));

		DirectX::XMStoreFloat4x4(&m_LocalTransform, localMatrix);

		if (m_pParent)
		{
			const auto worldMatrix = DirectX::XMMatrixMultiply(localMatrix, m_pParent->GetMatrix());
			DirectX::XMStoreFloat4x4(&m_Transform, worldMatrix);
		}
		else
		{
			m_Transform = m_LocalTransform;
		}

		for (auto pChild : m_Children)
			pChild->updateTransform();
	}
}