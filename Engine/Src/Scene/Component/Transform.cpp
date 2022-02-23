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
		m_LocalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalScale	= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	}

	DirectX::XMFLOAT3 Transform::GetPosition()
	{
		if (!HasParent())
			return m_LocalPosition;

		auto vecLocalPos = DirectX::XMLoadFloat3(&m_LocalPosition);
		auto parentMat = GetParent()->GetMatrix();
		auto matParentMat = DirectX::XMLoadFloat4x4(&parentMat);

		vecLocalPos = XMVector3Transform(vecLocalPos, matParentMat);

		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, vecLocalPos);

		return pos;
	}

	void Transform::SetPosition(DirectX::XMFLOAT3 pos)
	{
		if (!HasParent())
			m_LocalPosition = pos;

		auto vecPos = DirectX::XMLoadFloat3(&pos);
		auto parentMat = GetParent()->GetMatrix();
		auto matParentMat = DirectX::XMLoadFloat4x4(&parentMat);

		auto vecLocalPos = DirectX::XMVector3Transform(vecPos, DirectX::XMMatrixInverse(nullptr, matParentMat));

		DirectX::XMStoreFloat3(&m_LocalPosition, vecLocalPos);
	}

	DirectX::XMFLOAT3 Transform::GetRotation()
	{
		if (!HasParent())
			return m_LocalRotation;
		
		// 오일러각으로 출력해야 한다.

		return DirectX::XMFLOAT3();
	}

	void Transform::SetRotation(DirectX::XMFLOAT3 rot)
	{
		// 오일러각을 받아 회전시켜야 한다.
	}

	DirectX::XMFLOAT4X4 Transform::GetLocalMatrix()
	{
		auto vecLocalPos = DirectX::XMLoadFloat3(&m_LocalPosition);
		float radianX = DirectX::XMConvertToRadians(m_LocalRotation.x);
		float radianY = DirectX::XMConvertToRadians(m_LocalRotation.y);
		float radianZ = DirectX::XMConvertToRadians(m_LocalRotation.z);
		auto vecLocalRot = DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ);
		auto vecLocalScl = DirectX::XMLoadFloat3(&m_LocalScale);

		auto matLocalPos = DirectX::XMMatrixTranslationFromVector(vecLocalPos);
		auto matLocalRot = DirectX::XMMatrixRotationQuaternion(vecLocalRot);
		auto matLocalScl = DirectX::XMMatrixScalingFromVector(vecLocalScl);

		auto matLocalMat = matLocalScl * matLocalRot * matLocalPos;

		DirectX::XMFLOAT4X4 localMat;
		DirectX::XMStoreFloat4x4(&localMat, matLocalMat);

		return localMat;
	}

	DirectX::XMFLOAT4X4 Transform::GetMatrix()
	{
		if (!HasParent())
			return GetLocalMatrix();

		auto localMat = this->GetLocalMatrix();
		auto matLocalMat = DirectX::XMLoadFloat4x4(&localMat);
		auto parentMat = this->GetParent()->GetMatrix();
		auto matParentMat = DirectX::XMLoadFloat4x4(&parentMat);

		DirectX::XMFLOAT4X4 mat;
		DirectX::XMStoreFloat4x4(&mat, (matLocalMat * matParentMat));

		return mat;
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