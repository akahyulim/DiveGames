#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"

namespace dive
{
	Transform::Transform(GameObject* gameObject)
		: Component(typeid(Transform).hash_code(), gameObject, this)
	{
		m_LocalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		DirectX::XMStoreFloat4x4(&m_Matrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_LocalMatrix, DirectX::XMMatrixIdentity());

		m_LookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // 이건 추후 바꿔야 한다.
	}

	Transform::~Transform()
	{
	}

	void Transform::Update(float deltaTime)
	{
	}

	void Transform::Serialize(FileStream* fileStream)
	{
		// local pos, rot ,scl
		// lookAt
		fileStream->Write(m_Parent ? m_Parent->GetGameObject()->GetInstanceID() : 0);
	}

	void Transform::Deserialize(FileStream* fileStream)
	{
		// local pos, rot ,scl
		// lookAt
		unsigned int parentId = 0;
		fileStream->Read(&parentId);

		// update transform
	}

	void Transform::UpdateTransform()
	{
		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(GetLocalPositionVector());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX matLocalScl = DirectX::XMMatrixIdentity();

		auto localMatrix = matLocalScl * matLocalRot * matLocalPos;
		DirectX::XMStoreFloat4x4(&m_LocalMatrix, localMatrix);

		if (HasParent())
		{
			DirectX::XMStoreFloat4x4(&m_Matrix, (localMatrix * m_Parent->GetMatrix()));
		}
		else
		{
			m_Matrix = m_LocalMatrix;
		}

		for (auto child : m_Children)
		{
			child->UpdateTransform();
		}
	}

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		return DirectX::XMVectorSet(m_Matrix._41, m_Matrix._42, m_Matrix._43, 1.0f);
	}

	DirectX::XMFLOAT3 Transform::GetPositionFloat3() const
	{
		return DirectX::XMFLOAT3(m_Matrix._41, m_Matrix._42, m_Matrix._43);
	}

	void Transform::GetPosition(float& outX, float& outY, float& outZ) const
	{
		outX = m_Matrix._41;
		outY = m_Matrix._42;
		outZ = m_Matrix._43;
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

	void Transform::SetPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetPositionVector()))
			return;

		SetLocalPositionByVector(HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, m_Parent->GetMatrix()))
			: position);
	}

	void Transform::SetPositionByFloat3(const DirectX::XMFLOAT3& position)
	{
		SetPositionByVector(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPositionByVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetLocalPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetLocalPositionVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalPosition, position);

		UpdateTransform();
	}

	void Transform::SetLocalPositionByFloat3(const DirectX::XMFLOAT3& position)
	{;
		SetLocalPositionByVector(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{;
		SetLocalPositionByVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetLookAt(float x, float y, float z)
	{
		m_LookAt = DirectX::XMFLOAT3(x, y, z);
	}
}