#include "divepch.h"
#include "Transform.h"
#include "Math/Math.h"

namespace Dive
{
	Transform::Transform(GameObject* pGameObject)
		: Component(pGameObject)
	{
		Clear();
	}

	void Transform::Clear()
	{
		m_LocalTranslation	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation		= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LocalScale		= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		DirectX::XMStoreFloat4x4(&m_Matrix, DirectX::XMMatrixIdentity());
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		return *((DirectX::XMFLOAT3*)&m_Matrix._41);
	}

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		return DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)&m_Matrix._41);
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& trans)
	{
		auto currentPosition = GetPosition();
		if ((currentPosition.x == trans.x) && (currentPosition.y == trans.y) && (currentPosition.z == trans.z))
			return;

		auto newPosition = trans;

		if (HasParent())
		{
			auto vecPosition = DirectX::XMVector3Transform(
				DirectX::XMLoadFloat3(&newPosition), DirectX::XMMatrixInverse(nullptr, GetParent()->GetMatrix())
			);

			DirectX::XMStoreFloat3(&newPosition, vecPosition);
		}

		SetLocalPosition(newPosition);
	}

	DirectX::XMFLOAT4 Transform::GetRotation() const
	{
		DirectX::XMFLOAT4 rotQuat;
		DirectX::XMStoreFloat4(&rotQuat, GetRotationVector());

		return rotQuat;
	}

	DirectX::XMVECTOR Transform::GetRotationVector() const
	{
		DirectX::XMVECTOR trans, rotQuat, scale;
		DirectX::XMMatrixDecompose(&scale, &rotQuat, &trans, DirectX::XMLoadFloat4x4(&m_Matrix));

		return rotQuat;
	}

	DirectX::XMFLOAT3 Transform::GetRotationDegrees() const
	{
		return Math::QuaternionToEularDegrees(GetRotation());
	}

	void Transform::SetRotation(const DirectX::XMFLOAT4& rotQuat)
	{
		auto curRotQuat = GetRotationVector();
		auto rotQuatVector = DirectX::XMLoadFloat4(&rotQuat);
		if (DirectX::XMQuaternionEqual(curRotQuat, rotQuatVector))
			return;

		if (HasParent())
		{
			auto parentRotQuat = GetParent()->GetRotationVector();
			rotQuatVector = DirectX::XMQuaternionMultiply(rotQuatVector, DirectX::XMQuaternionInverse(parentRotQuat));
		}

		DirectX::XMFLOAT4 newRotQuat;
		DirectX::XMStoreFloat4(&newRotQuat, rotQuatVector);

		SetLocalRotation(newRotQuat);
	}

	void Transform::SetRotation(const DirectX::XMFLOAT3& degrees)
	{
		auto radianX = DirectX::XMConvertToRadians(degrees.x);
		auto radianY = DirectX::XMConvertToRadians(degrees.y);
		auto radianZ = DirectX::XMConvertToRadians(degrees.z);
		
		auto rotQuatVector = DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ);

		auto curRotQuat = GetRotationVector();
		if (DirectX::XMQuaternionEqual(curRotQuat, rotQuatVector))
			return;

		if (HasParent())
		{
			auto parentRotQuat = GetParent()->GetRotationVector();
			rotQuatVector = DirectX::XMQuaternionMultiply(rotQuatVector, DirectX::XMQuaternionInverse(parentRotQuat));
		}

		DirectX::XMFLOAT4 newRotQuat;
		DirectX::XMStoreFloat4(&newRotQuat, rotQuatVector);

		SetLocalRotation(newRotQuat);
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotationDegrees() const
	{
		return Math::QuaternionToEularDegrees(m_LocalRotation);
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT3& degrees)
	{
		auto radianX = DirectX::XMConvertToRadians(degrees.x);
		auto radianY = DirectX::XMConvertToRadians(degrees.y);
		auto radianZ = DirectX::XMConvertToRadians(degrees.z);

		DirectX::XMStoreFloat4(&m_LocalRotation, DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, GetScaleVector());

		return scale;
	}

	DirectX::XMVECTOR Transform::GetScaleVector() const
	{
		DirectX::XMVECTOR trans, rotQuat, scale;
		DirectX::XMMatrixDecompose(&scale, &rotQuat, &trans, DirectX::XMLoadFloat4x4(&m_Matrix));

		return scale;
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		auto currentScale = GetScale();
		if ((currentScale.x == scale.x) && (currentScale.y == scale.y) && (currentScale.z == scale.z))
			return;

		auto newScale = scale;

		if (HasParent())
		{
			auto parentScale = GetParent()->GetScale();
			newScale.x /= parentScale.x;
			newScale.y /= parentScale.y;
			newScale.z /= parentScale.z;
		}

		SetLocalScale(newScale);
	}

	DirectX::XMMATRIX Transform::GetMatrix()
	{
		auto matrix = GetLocalMatrix();

		if (HasParent())
		{
			matrix *= GetParent()->GetMatrix();
		}

		DirectX::XMStoreFloat4x4(&m_Matrix, matrix);
		
		return matrix;
	}

	DirectX::XMMATRIX Transform::GetLocalMatrix() const
	{
		auto vecLocalScale = DirectX::XMLoadFloat3(&m_LocalScale);
		auto vecLocalRotation = DirectX::XMLoadFloat4(&m_LocalRotation);
		auto vecLocalTranslation = DirectX::XMLoadFloat3(&m_LocalTranslation);

		return DirectX::XMMatrixScalingFromVector(vecLocalScale) * DirectX::XMMatrixRotationQuaternion(vecLocalRotation)
			* DirectX::XMMatrixTranslationFromVector(vecLocalTranslation);
	}

	void Transform::Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo)
	{
		auto trans = translation;

		if (relativeTo == eSpace::Self)
		{
			if (HasParent())
			{
				auto transVector = DirectX::XMVector3Transform(
					DirectX::XMLoadFloat3(&trans), DirectX::XMMatrixInverse(nullptr, GetParent()->GetMatrix()));

				DirectX::XMStoreFloat3(&trans, transVector);
			}
		}

		m_LocalTranslation.x += trans.x;
		m_LocalTranslation.y += trans.y;
		m_LocalTranslation.z += trans.z;
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		Translate(DirectX::XMFLOAT3(x, y, z), relativeTo);
	}

	void Transform::Rotate(const DirectX::XMFLOAT3& degrees, eSpace relativeTo)
	{
		auto radianX = DirectX::XMConvertToRadians(degrees.x);
		auto radianY = DirectX::XMConvertToRadians(degrees.y);
		auto radianZ = DirectX::XMConvertToRadians(degrees.z);

		Rotate(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ), relativeTo);
	}

	// 아직 잘 모르겠다.
	void Transform::Rotate(const DirectX::XMVECTOR& rotQuat, eSpace relativeTo)
	{
		auto rotQuatVector = rotQuat;

		if (relativeTo == eSpace::Self)
		{
			if (HasParent())
			{
				auto parentRotQuatVector = GetParent()->GetRotationVector();

				// rotQuatVector = 부모 회전 사원수 역 * rotQuatVector * 부모 회전 사원수?
			}
		}

		// 로컬 * rotQuatVector
	}

	void Transform::Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo)
	{
		auto radianX = DirectX::XMConvertToRadians(xAngle);
		auto radianY = DirectX::XMConvertToRadians(yAngle);
		auto radianZ = DirectX::XMConvertToRadians(zAngle);

		Rotate(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ), relativeTo);
	}

	void Transform::Rotate(DirectX::XMFLOAT3 axis, float angle, eSpace relativeTo)
	{
		// axis 축을 angle만큼 회전시킨 후 다시 m_LocalRotation을 회전하는 것 같다.
	}

	// 위치 행렬 * 회전 행렬에 축 벡터를 곱하면 될 것 같다.
	DirectX::XMFLOAT3 Transform::GetForward()
	{
		return DirectX::XMFLOAT3();
	}

	DirectX::XMFLOAT3 Transform::GetUp()
	{
		return DirectX::XMFLOAT3();
	}

	DirectX::XMFLOAT3 Transform::GetRight()
	{
		return DirectX::XMFLOAT3();
	}

	void Transform::LookAt(Transform target, DirectX::XMFLOAT3 worldUp)
	{
		// 회전시켜서 target의 현재위치를 전방 벡터로 한다.
	}

	// 유니티의 경우 부모에게서 독립하면
	// 월드 좌표계가 로컬 좌표계가 된다...?
	// 그리고 현재 루트가 부모에게 들어갈 때
	// 부모의 변환이 그대로 적용되어 버린다. 
	// 구현상으로는 당연한데...
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
				// test
				{
					m_LocalTranslation = GetPosition();
					m_LocalRotation = GetRotation();
					m_LocalScale = GetScale();
				}

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
			pChild->SetParent(nullptr);
		}

		m_Children.clear();
		m_Children.shrink_to_fit();
	}
}