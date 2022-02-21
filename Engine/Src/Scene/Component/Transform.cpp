#include "divepch.h"
#include "Transform.h"

namespace Dive
{
	Transform::Transform(GameObject* pGameObject)
		: Component(pGameObject)
	{
		Clear();
	}

	void Transform::Clear()
	{
		m_LocalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LocalScale	= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	}

	DirectX::XMFLOAT3 Transform::GetLocalPosition() const
	{
		return m_LocalPosition;
	}

	DirectX::XMVECTOR Transform::GetLocalPositionVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalPosition);
	}

	void Transform::GetLocalPosition(float& outX, float& outY, float& outZ) const
	{
		outX = m_LocalPosition.x;
		outY = m_LocalPosition.y;
		outZ = m_LocalPosition.z;
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		SetLocalPosition(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(const DirectX::XMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetLocalPositionVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalPosition, position);
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPosition(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	DirectX::XMFLOAT3 Transform::GetPosition()
	{
		auto matrix = GetMatrix4X4();

		return DirectX::XMFLOAT3(
			matrix._41,
			matrix._42,
			matrix._43
		);
	}

	DirectX::XMVECTOR Transform::GetPositionVector()
	{
		auto matrix = GetMatrix4X4();

		return DirectX::XMVectorSet(
			matrix._41,
			matrix._42,
			matrix._43,
			1.0f
		);
	}

	void Transform::GetPosition(float& outX, float& outY, float& outZ)
	{
		auto matrix = GetMatrix4X4();

		outX = matrix._41;
		outZ = matrix._42;
		outY = matrix._43;
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		SetPosition(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetPosition(const DirectX::XMVECTOR& position)
	{
		SetLocalPosition(
			HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, GetParent()->GetMatrix())) :
			position
		);
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPosition(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	DirectX::XMVECTOR Transform::GetLocalRotationVector() const
	{
		return DirectX::XMVECTOR();
	}

	DirectX::XMVECTOR Transform::GetLocalScaleVector() const
	{
		return DirectX::XMVECTOR();
	}

	DirectX::XMFLOAT4X4 Transform::GetLocalMatrix4X4()
	{
		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(GetLocalPositionVector());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixRotationQuaternion(GetLocalRotationVector());
		DirectX::XMMATRIX matLocalScl = DirectX::XMMatrixScalingFromVector(GetLocalScaleVector());

		auto localMatrix = matLocalScl * matLocalRot * matLocalPos;
		DirectX::XMStoreFloat4x4(&m_LocalMatrix, localMatrix);
		
		return m_LocalMatrix;
	}

	DirectX::XMMATRIX Transform::GetLocalMatrix()
	{
		auto matrix = GetLocalMatrix4X4();
		return DirectX::XMLoadFloat4x4(&matrix);
	}

	DirectX::XMFLOAT4X4 Transform::GetMatrix4X4()
	{
		if(HasParent())
		{
			DirectX::XMStoreFloat4x4(&m_Matrix, (GetLocalMatrix() * m_pParent->GetMatrix()));
		}
		else
		{
			m_Matrix = GetLocalMatrix4X4();
		}

		return m_Matrix;
	}

	DirectX::XMMATRIX Transform::GetMatrix()
	{
		auto matrix = GetMatrix4X4();
		return DirectX::XMLoadFloat4x4(&matrix);
	}

	void Transform::SetParent(Transform* pParent)
	{
		if (pParent)
		{
			if (pParent->GetInstanceID() == GetInstanceID())
				return;
		}

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

		if (pParent)
		{
			pParent->m_Children.emplace_back(this);
		}

		SetParent(pParent);
	}

	Transform* Transform::GetRoot()
	{
		if (m_pParent == nullptr)
			return this;

		return m_pParent->GetRoot();
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
			pChild->SetParent(nullptr);
		}

		m_Children.clear();
		m_Children.shrink_to_fit();
	}
}