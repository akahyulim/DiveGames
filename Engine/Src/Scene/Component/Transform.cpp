#include "divepch.h"
#include "Transform.h"

#include "Scene/GameObject.h" // test

namespace Dive
{
	Transform::Transform(GameObject* pGameObject)
		: Component(pGameObject)
	{
		m_Type = eComponentType::Transform;
	}

	void Transform::Clear()
	{
		m_Position	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_Rotation	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_Scale		= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		return m_Position;
	}

	void Transform::SetPosition(DirectX::XMFLOAT3 pos)
	{
		m_Position = pos;
	}

	DirectX::XMFLOAT3 Transform::GetLocalPosition()
	{
		if (!HasParent())
			return m_Position;

		auto parentPos = GetParent()->GetPosition();
		auto vecParentPos = DirectX::XMLoadFloat3(&parentPos);
		auto vecPos = DirectX::XMLoadFloat3(&m_Position);
		auto vecLocalPos = DirectX::XMVectorSubtract(vecPos, vecParentPos);

		DirectX::XMFLOAT3 localPos;
		DirectX::XMStoreFloat3(&localPos, vecLocalPos);

		return localPos;
	}

	void Transform::SetLocalPosition(DirectX::XMFLOAT3 pos)
	{
		if (!HasParent())
		{
			m_Position = pos;
			return;
		}

		auto parentPos = GetParent()->GetPosition();
		auto vecParentPos = DirectX::XMLoadFloat3(&parentPos);
		auto vecLocalPos = DirectX::XMLoadFloat3(&pos);
		auto vecPos = DirectX::XMVectorAdd(vecParentPos, vecLocalPos);

		DirectX::XMStoreFloat3(&m_Position, vecPos);
	}

	DirectX::XMFLOAT3 Transform::GetRotation() const
	{
		return m_Rotation;
	}

	void Transform::SetRotation(DirectX::XMFLOAT3 rot)
	{
		m_Rotation = rot;
	}

	// 이것도 유니티 구현 확인이 필요하다.
	DirectX::XMFLOAT3 Transform::GetLocalRotation()
	{
		if (!HasParent())
			return m_Rotation;

		auto parentRot = GetParent()->GetRotation();
		auto vecParentRot = DirectX::XMLoadFloat3(&parentRot);
		auto vecRot = DirectX::XMLoadFloat3(&m_Rotation);
		auto vecLocalRot = DirectX::XMVectorSubtract(vecRot, vecParentRot);

		DirectX::XMFLOAT3 localRot;
		DirectX::XMStoreFloat3(&localRot, vecLocalRot);

		return localRot;
	}

	void Transform::SetLocalRotation(DirectX::XMFLOAT3 rot)
	{
		if (!HasParent())
		{
			m_Rotation = rot;
			return;
		}

		auto parentRot = GetParent()->GetRotation();
		auto vecParentRot = DirectX::XMLoadFloat3(&parentRot);
		auto vecLocalRot = DirectX::XMLoadFloat3(&rot);
		auto vecRot = DirectX::XMVectorAdd(vecParentRot, vecLocalRot);

		DirectX::XMStoreFloat3(&m_Rotation, vecRot);
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		return m_Scale;
	}

	void Transform::SetScale(DirectX::XMFLOAT3 scl)
	{
		m_Scale = scl;
	}

	// 빼기가 맞는지, 나누기인지 확인 필요
	DirectX::XMFLOAT3 Transform::GetLocalScale()
	{
		if (!HasParent())
			return m_Scale;

		auto parentScl = GetParent()->GetScale();
		auto vecParentScl = DirectX::XMLoadFloat3(&parentScl);
		auto vecScl = DirectX::XMLoadFloat3(&m_Scale);
		auto vecLocalScl = DirectX::XMVectorSubtract(vecScl, vecParentScl);

		DirectX::XMFLOAT3 localScl;
		DirectX::XMStoreFloat3(&localScl, vecLocalScl);

		return localScl;
	}

	void Transform::SetLocalScale(DirectX::XMFLOAT3 scl)
	{
		if (!HasParent())
		{
			m_Scale = scl;
			return;
		}

		auto parentScl = GetParent()->GetScale();
		auto vecParentScl = DirectX::XMLoadFloat3(&parentScl);
		auto vecLocalScl = DirectX::XMLoadFloat3(&scl);
		auto vecScl = DirectX::XMVectorAdd(vecParentScl, vecLocalScl);

		DirectX::XMStoreFloat3(&m_Scale, vecScl);
	}

	DirectX::XMFLOAT4X4 Transform::GetMatrix()
	{
		auto vecPos = DirectX::XMLoadFloat3(&m_Position);
		float radianX = DirectX::XMConvertToRadians(m_Rotation.x);
		float radianY = DirectX::XMConvertToRadians(m_Rotation.y);
		float radianZ = DirectX::XMConvertToRadians(m_Rotation.z);
		auto vecRot = DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ);
		auto vecScl = DirectX::XMLoadFloat3(&m_Scale);

		auto matPos = DirectX::XMMatrixTranslationFromVector(vecPos);
		auto matRot = DirectX::XMMatrixRotationQuaternion(vecRot);
		auto matScl = DirectX::XMMatrixScalingFromVector(vecScl);

		auto matMat = matScl * matRot * matPos;

		DirectX::XMFLOAT4X4 mat;
		DirectX::XMStoreFloat4x4(&mat, matMat);

		return mat;
	}

	DirectX::XMFLOAT4X4 Transform::GetLocalMatrix()
	{
		return DirectX::XMFLOAT4X4();
	}

	void Transform::SetParent(Transform* pParent)
	{
		if (pParent)
		{
			if (pParent->GetInstanceID() == GetInstanceID())
				return;

			if (HasParent())
			{
				if (pParent->GetInstanceID() == GetParent()->GetInstanceID())
					return;

				auto& sibling = GetParent()->m_Children;
				for (auto it = sibling.begin(); it != sibling.end();)
				{
					if ((*it)->GetInstanceID() == GetInstanceID())
					{
						sibling.erase(it);
						break;
					}
					else
					{
						it++;
					}
				}
			}

			pParent->m_Children.emplace_back(this);
		}
		else
		{
			if (HasParent())
			{
				auto& sibling = GetParent()->m_Children;
				for (auto it = sibling.begin(); it != sibling.end();)
				{
					if ((*it)->GetInstanceID() == GetInstanceID())
					{
						sibling.erase(it);
						break;
					}
					else
					{
						it++;
					}
				}
			}
		}

		m_pParent = pParent;

		// test
		auto wp = this->GetPosition();
		DV_CORE_TRACE("{0:s}'s world position ({1:f}, {2:f}, {3:f})", this->GetGameObject()->GetName(), wp.x, wp.y, wp.z);
	}

	Transform* Transform::GetRoot()
	{
		if (m_pParent == nullptr)
			return this;

		return m_pParent->GetRoot();
	}

	bool Transform::HasChildren() const
	{
		return !m_Children.empty();
	}

	std::vector<Transform*> Transform::GetChildren() const
	{
		return m_Children;
	}

	unsigned int Transform::GetChildCount()
	{
		return static_cast<unsigned int>(m_Children.size());
	}

	Transform* Transform::GetChild(int index)
	{
		if ((m_Children.empty()) || (index > static_cast<int>(m_Children.size())))
			return nullptr;

		return m_Children[index];
	}

	// 전달받은 Parent가 자신과 같아도 true
	bool Transform::IsChildOf(const Transform* pParent)
	{
		if (pParent == nullptr)
			return false;

		if (!HasParent())
			return false;

		if (this->GetInstanceID() == pParent->GetInstanceID())
			return true;

		for (auto pChild : m_Children)
		{
			if (this->GetInstanceID() == pChild->GetInstanceID())
				return true;
		}

		return m_pParent->IsChildOf(pParent);
	}

	// 자신의 자식을 모두 독립
	void Transform::DetachChildren()
	{
		if (m_Children.empty())
			return;

		for (auto pChild : m_Children)
		{
			pChild->m_pParent = nullptr;
		}

		m_Children.clear();
		m_Children.shrink_to_fit();
	}
}