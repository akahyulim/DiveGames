#include "divepch.h"
#include "Transform.h"
#include "Scene/GameObject.h"

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
	}

	// nullptr을 반환하지 않습니다.
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

		// 유니티에서 가져온 인터페이스다.
		// 그런데 유니티엔 Gets/SetSiblingIndex()라는게 있다.
		// vector index와는 동일한 의미인 듯 한데
		// 이를 Set, Get하는 방법을 찾아야 한다. (Set은 에바다...)
		return m_Children[index];
	}

	bool Transform::IsChildOf(const Transform* pParent)
	{
		if (pParent == nullptr)
			return false;

		// GameObject의 ID로 비교해야 한다.
		// 유니티의 경우 Component도 Object를 상속하므로 고유 InstanceID를 가진다.
		// 실제로 GameObject로 비교하는 것 보다 직접 비교하는 편이 낫다.

		return false;
	}

	// 자신의 자식을 모두 독립시킵니다.
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