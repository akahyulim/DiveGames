#include "divepch.h"
#include "GameObject.h"
#include "Core/CoreDefs.h"
#include "Math/Math.h"
#include "Components/Component.h"

namespace Dive
{
	GameObject::GameObject(Scene* pScene, const std::string& name)
		: m_Name(name)
		, m_ID(0)
		, m_pScene(pScene)
		, m_bActive(true)
		, m_bMarkedTarget(false)
		, m_pParent(nullptr)
	{
		m_LocalPosition = { 0.0f, 0.0f, 0.0f };
		m_LocalRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_LocalScale = { 1.0f, 1.0f, 1.0f };

		DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_LocalTransform, DirectX::XMMatrixIdentity());
	}

	GameObject::~GameObject()
	{
		for (auto& it : m_Components)
			DV_DELETE(it.second);

		DV_ENGINE_TRACE("게임오브젝트({0:s}, {1:d}) 소멸", GetName(), GetID());
	}

	void GameObject::Update()
	{
		if (!m_bActive)
			return;

		for (auto& it : m_Components)
			it.second->Update();
	}

	bool GameObject::HashComponent(size_t typeHash) const
	{
		auto it = m_Components.find(typeHash);
		return it != m_Components.end();
	}

	Component* GameObject::GetComponent(size_t typeHash) const
	{
		auto it = m_Components.find(typeHash);
		if (it != m_Components.end())
			return it->second;

		return nullptr;
	}

	DirectX::XMFLOAT3 GameObject::GetPosition() const
	{
		return { m_Transform._41, m_Transform._42, m_Transform._43 };
	}

	DirectX::XMVECTOR GameObject::GetPositionVector() const
	{
		return DirectX::XMVectorSet(m_Transform._41, m_Transform._42, m_Transform._43, 1.0f);
	}

	void GameObject::SetPosition(float posX, float posY, float posZ)
	{
		SetPosition(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void GameObject::SetPosition(const DirectX::XMFLOAT3& position)
	{
		SetPosition(DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f));
	}

	void GameObject::SetPosition(const DirectX::XMVECTOR& position)
	{
		DirectX::XMVECTOR localPosition = position;

		if (m_pParent)
		{
			const auto parentWorldTransformInverse = DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix());
			localPosition = DirectX::XMVector3Transform(position, parentWorldTransformInverse);
		}

		SetLocalPosition(DirectX::XMVectorGetX(localPosition), DirectX::XMVectorGetY(localPosition), DirectX::XMVectorGetZ(localPosition));
	}
	
	DirectX::XMVECTOR GameObject::GetLocalPositionVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalPosition);
	}

	void GameObject::SetLocalPosition(float posX, float posY, float posZ)
	{
		SetLocalPosition(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void GameObject::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		SetLocalPosition(DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f));
	}

	void GameObject::SetLocalPosition(const DirectX::XMVECTOR& position)
	{
		if (!DirectX::XMVector3Equal(GetLocalPositionVector(), position))
		{
			DirectX::XMStoreFloat3(&m_LocalPosition, position);
			updateTransform();
		}
	}
	
	DirectX::XMFLOAT3 GameObject::GetRotationDegrees() const
	{
		return Math::QuaternionToEuler(GetRotationQuaternionVector());
	}

	DirectX::XMFLOAT4 GameObject::GetRotationQuaternion() const
	{
		const auto rotation = GetRotationQuaternionVector();
		return { DirectX::XMVectorGetX(rotation),DirectX::XMVectorGetY(rotation), DirectX::XMVectorGetZ(rotation), DirectX::XMVectorGetW(rotation) };
	}

	DirectX::XMVECTOR GameObject::GetRotationQuaternionVector() const
	{
		DirectX::XMVECTOR position, rotation, scale;
		DirectX::XMMatrixDecompose(&scale, &rotation, &position, DirectX::XMLoadFloat4x4(&m_Transform));

		return rotation;
	}


	void GameObject::SetRotation(float degreeX, float degreeY, float degreeZ)
	{
		const float roll = DirectX::XMConvertToRadians(degreeX);
		const float pitch = DirectX::XMConvertToRadians(degreeY);
		const float yaw = DirectX::XMConvertToRadians(degreeZ);

		const auto worldRotation = DirectX::XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);

		SetRotation(worldRotation);
	}

	void GameObject::SetRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotation(DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w));
	}

	void GameObject::SetRotation(const DirectX::XMVECTOR& quaternion)
	{
		DirectX::XMVECTOR localRotation = quaternion;

		if (m_pParent)
		{
			const auto parentWorldRotationInverse = DirectX::XMQuaternionInverse(m_pParent->GetRotationQuaternionVector());
			localRotation = DirectX::XMQuaternionMultiply(parentWorldRotationInverse, quaternion);
		}

		SetLocalRotation(localRotation);
	}

	DirectX::XMFLOAT3 GameObject::GetLocalRotationDegrees() const
	{
		return Math::QuaternionToEuler(GetLocalRotationQuaternionVector());
	}
	
	DirectX::XMVECTOR GameObject::GetLocalRotationQuaternionVector() const
	{
		return DirectX::XMLoadFloat4(&m_LocalRotation);
	}

	void GameObject::SetLocalRotation(float degreeX, float degreeY, float degreeZ)
	{
		float roll = DirectX::XMConvertToRadians(degreeX);
		float pitch = DirectX::XMConvertToRadians(degreeY);
		float yaw = DirectX::XMConvertToRadians(degreeZ);

		auto localRotation = DirectX::XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);
		
		SetLocalRotation(localRotation);
	}

	void GameObject::SetLocalRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotation(DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w));
	}

	void GameObject::SetLocalRotation(const DirectX::XMVECTOR& quaternion)
	{
		if (!DirectX::XMVector4Equal(GetRotationQuaternionVector(), quaternion))
		{
			DirectX::XMStoreFloat4(&m_LocalRotation, quaternion);
			updateTransform();
		}
	}
	
	DirectX::XMFLOAT3 GameObject::GetScale() const
	{
		const auto scale = GetScaleVector();
		return { DirectX::XMVectorGetX(scale), DirectX::XMVectorGetY(scale), DirectX::XMVectorGetZ(scale) };
	}

	DirectX::XMVECTOR GameObject::GetScaleVector() const
	{
		DirectX::XMVECTOR position, rotation, scale;
		DirectX::XMMatrixDecompose(&scale, &rotation, &position, DirectX::XMLoadFloat4x4(&m_Transform));

		return scale;
	}

	void GameObject::SetScale(float sclX, float sclY, float sclZ)
	{
		SetScale(DirectX::XMVectorSet(sclX, sclY, sclZ, 1.0f));
	}

	void GameObject::SetScale(const DirectX::XMFLOAT3& scale)
	{
		SetScale(DirectX::XMVectorSet(scale.x, scale.y, scale.z, 1.0f));
	}

	void GameObject::SetScale(const DirectX::XMVECTOR& scale)
	{
		DirectX::XMVECTOR localScale = scale;

		if (m_pParent)
		{
			const auto parentScale = m_pParent->GetScaleVector();
			localScale = DirectX::XMVectorDivide(scale, parentScale);
		}

		SetLocalScale(localScale);
	}
	
	DirectX::XMVECTOR GameObject::GetLocalScaleVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalScale);
	}

	void GameObject::SetLocalScale(float sclX, float sclY, float sclZ)
	{
		SetLocalScale(DirectX::XMVectorSet(sclX, sclY, sclZ, 1.0f));
	}

	void GameObject::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		SetLocalScale(DirectX::XMVectorSet(scale.x, scale.y, scale.z, 1.0f));
	}

	void GameObject::SetLocalScale(const DirectX::XMVECTOR& scale)
	{
		if (!DirectX::XMVector3Equal(GetScaleVector(), scale))
		{
			DirectX::XMStoreFloat3(&m_LocalScale, scale);
			updateTransform();
		}
	}

	void GameObject::Translate(float x, float y, float z, eSpace relativeTo)
	{
		if (relativeTo == eSpace::World)
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
	void GameObject::Rotate(const DirectX::XMVECTOR& quaternion, eSpace relativeTo)
	{
		auto currentWorldRotationQuaternion = DirectX::XMLoadFloat4(&m_LocalRotation);

		if (relativeTo == eSpace::World)
		{
			auto newWorldRotationQuaternion = DirectX::XMQuaternionMultiply(currentWorldRotationQuaternion, quaternion);
			DirectX::XMStoreFloat4(&m_LocalRotation, newWorldRotationQuaternion);
		}
		else
		{
			auto newWorldRotationQuaternion = DirectX::XMQuaternionMultiply(quaternion, currentWorldRotationQuaternion);
			DirectX::XMStoreFloat4(&m_LocalRotation, newWorldRotationQuaternion);
		}

		updateTransform();
	}

	// Translate처럼 SetLocalRoatation()을 통해 updateTransform()을 수행토록 변경했다.
	// 특별한 처이점은 없다.
	void GameObject::Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo)
	{
		float roll = DirectX::XMConvertToRadians(degreeX);
		float pitch = DirectX::XMConvertToRadians(degreeY);
		float yaw = DirectX::XMConvertToRadians(degreeZ);

		auto rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);
		auto currentWorldRotationQuaternion = DirectX::XMLoadFloat4(&m_LocalRotation);

		if (relativeTo == eSpace::World)
		{	
			auto newWorldRotationQuaternion = DirectX::XMQuaternionMultiply(currentWorldRotationQuaternion, rotationQuaternion);
			SetLocalRotation(newWorldRotationQuaternion);
			//DirectX::XMStoreFloat4(&m_LocalRotation, newWorldRotationQuaternion);
		}
		else
		{
			auto newWorldRotationQuaternion = DirectX::XMQuaternionMultiply(rotationQuaternion, currentWorldRotationQuaternion);
			SetLocalRotation(newWorldRotationQuaternion);
			//DirectX::XMStoreFloat4(&m_LocalRotation, newWorldRotationQuaternion);
		}

		//updateTransform();
	}

	void GameObject::LookAt(float x, float y, float z, float upX, float upY, float upZ)
	{
		LookAt(DirectX::XMVectorSet(x, y, z, 1.0f), DirectX::XMVectorSet(upX, upY, upZ, 1.0f));
	}

	void GameObject::LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
	{
		LookAt(DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f), DirectX::XMVectorSet(up.x, up.y, up.z, 1.0f));
	}

	void GameObject::LookAt(const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& up)
	{
		if (DirectX::XMVector3Equal(target, GetPositionVector()))
		{
			DV_ENGINE_WARN("현재 위치를 바라볼 수 없습니다.");
			return;
		}

		auto forwardVector = DirectX::XMVectorSubtract(target, GetPositionVector());
		forwardVector = DirectX::XMVector3Normalize(forwardVector);

		if (DirectX::XMVector3NearEqual(DirectX::XMVector3Dot(forwardVector, up), DirectX::XMVector3Length(up), DirectX::XMVectorReplicate(0.0001f)))
		{
			DV_ENGINE_WARN("바라보는 대상과 상향 벡터가 평행하여 적용이 불가능합니다.");
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

	void GameObject::LookAt(const GameObject* pTarget, const DirectX::XMVECTOR& up)
	{
		if (!pTarget)
		{
			DV_ENGINE_WARN("바라보는 대상 게임오브젝트가 존재하지 않습니다.");
			return;
		}

		LookAt(pTarget->GetPositionVector(), up);
	}

	void GameObject::SetTransform(const DirectX::XMFLOAT4X4& world)
	{
		auto worldMatrix = DirectX::XMLoadFloat4x4(&world);
		SetMatrix(worldMatrix);
	}

	void GameObject::SetMatrix(const DirectX::XMMATRIX& world)
	{
		DirectX::XMMATRIX localMatrix = world;

		if (m_pParent)
		{
			const auto parentWorldMatrixInverse = DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix());
			localMatrix = DirectX::XMMatrixMultiply(parentWorldMatrixInverse, world);
		}

		SetLocalMatrix(localMatrix);
	}

	void GameObject::SetLocalTransform(const DirectX::XMFLOAT4X4& local)
	{
		auto localMatrix = DirectX::XMLoadFloat4x4(&local);
		SetLocalMatrix(localMatrix);
	}

	void GameObject::SetLocalMatrix(const DirectX::XMMATRIX& local)
	{
		DirectX::XMVECTOR position, rotation, scale;
		DirectX::XMMatrixDecompose(&scale, &rotation, &position, local);
		
		DirectX::XMStoreFloat3(&m_LocalPosition, position);
		DirectX::XMStoreFloat4(&m_LocalRotation, rotation);
		DirectX::XMStoreFloat3(&m_LocalScale, scale);

		updateTransform();
	}

	DirectX::XMFLOAT3 GameObject::GetForward() const
	{
		DirectX::XMFLOAT3 forward;
		DirectX::XMStoreFloat3(&forward, GetForwardVector());

		return forward;
	}

	DirectX::XMVECTOR GameObject::GetForwardVector() const
	{
		auto forward = DirectX::XMVectorSet(m_Transform._31, m_Transform._32, m_Transform._33, 0.0f);
		return DirectX::XMVector3Normalize(forward);
	}

	DirectX::XMFLOAT3 GameObject::GetUpward() const
	{
		DirectX::XMFLOAT3 upward;
		DirectX::XMStoreFloat3(&upward, GetUpwardVector());

		return upward;
	}

	DirectX::XMVECTOR GameObject::GetUpwardVector() const
	{
		auto upward = DirectX::XMVectorSet(m_Transform._21, m_Transform._22, m_Transform._23, 0.0f);
		return DirectX::XMVector3Normalize(upward);
	}

	DirectX::XMFLOAT3 GameObject::GetRightward() const
	{
		DirectX::XMFLOAT3 rightward;
		DirectX::XMStoreFloat3(&rightward, GetRightwardVector());

		return rightward;
	}

	DirectX::XMVECTOR GameObject::GetRightwardVector() const
	{
		auto rightward = DirectX::XMVectorSet(m_Transform._11, m_Transform._12, m_Transform._13, 0.0f);
		return DirectX::XMVector3Normalize(rightward);
	}
	
	void GameObject::SetParent(GameObject* pParent)
	{
		if (pParent)
		{
			if (pParent == this)
				return;

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

	GameObject* GameObject::GetRoot()
	{
		if (!m_pParent)
			return this;
		
		return m_pParent->GetRoot();
	}

	GameObject* GameObject::GetChild(uint32_t index) const
	{
		if(m_Children.empty() || index > (uint32_t)m_Children.size())
			return nullptr;

		return m_Children[index];
	}

	bool GameObject::IsChildOf(GameObject* pParent) const
	{
		DV_ENGINE_ASSERT(pParent);

		if (pParent == this)
			return true;

		if (!m_pParent)
			return false;
		else
			return m_pParent->IsChildOf(pParent);
	}

	void GameObject::DetachChildren()
	{
		for (auto* pChild : m_Children)
		{
			pChild->SetParent(nullptr);
		}
		m_Children.clear();
		m_Children.shrink_to_fit();
	}
	
	void GameObject::updateTransform()
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