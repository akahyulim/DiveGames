#include "divepch.h"
#include "Transform.h"

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

	// -360 ~ 360 ���̷� �����ϰ� �ʹ�.
	void Transform::SetRotation(DirectX::XMFLOAT3 rot)
	{
		m_Rotation = rot;
	}

	// �̰͵� ����Ƽ ���� Ȯ���� �ʿ��ϴ�.
	// ���� ���Ϸ� ���� �����ϰ� �ʹ�.
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

		// SetRotation()���� ������ �����ϴ� ������ �߰��Ѵٸ�
		// �� �κе� SetRotation()�� ������� �ٲٴ� �� ����.
		DirectX::XMStoreFloat3(&m_Rotation, vecRot);
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		return m_Scale;
	}

	void Transform::SetScale(DirectX::XMFLOAT3 scl)
	{
		if ((scl.x <= 0.0f) || (scl.y <= 0.0f) || (scl.z <= 0.0f))
			return;

		m_Scale = scl;
	}

	DirectX::XMFLOAT3 Transform::GetLocalScale()
	{
		if (!HasParent())
			return m_Scale;

		auto parentScl = GetParent()->GetScale();
		auto vecParentScl = DirectX::XMLoadFloat3(&parentScl);
		auto vecScl = DirectX::XMLoadFloat3(&m_Scale);
		auto vecLocalScl = DirectX::XMVectorDivide(vecScl, vecParentScl);

		DirectX::XMFLOAT3 localScl;
		DirectX::XMStoreFloat3(&localScl, vecLocalScl);

		return localScl;
	}

	void Transform::SetLocalScale(DirectX::XMFLOAT3 scl)
	{
		if ((scl.x <= 0.0f) || (scl.y <= 0.0f) || (scl.z <= 0.0f))
			return;

		if (!HasParent())
		{
			m_Scale = scl;
			return;
		}

		auto parentScl = GetParent()->GetScale();
		auto vecParentScl = DirectX::XMLoadFloat3(&parentScl);
		auto vecLocalScl = DirectX::XMLoadFloat3(&scl);
		auto vecScl = DirectX::XMVectorMultiply(vecParentScl, vecLocalScl);

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
		if (!HasParent())
			return GetMatrix();

		auto mat = GetMatrix();
		auto matMat = DirectX::XMLoadFloat4x4(&mat);
		auto parentMat = GetParent()->GetMatrix();
		auto matInverseParentMat = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&parentMat));
		auto matLocalMat = matMat * matInverseParentMat;

		DirectX::XMFLOAT4X4 localMat;
		DirectX::XMStoreFloat4x4(&localMat, matLocalMat);

		return localMat;
	}

	DirectX::XMFLOAT4X4 Transform::GetRotationMatrix()
	{
		float radianX = DirectX::XMConvertToRadians(m_Rotation.x);
		float radianY = DirectX::XMConvertToRadians(m_Rotation.y);
		float radianZ = DirectX::XMConvertToRadians(m_Rotation.z);
		auto vecRot = DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ);
		auto matRotMat = DirectX::XMMatrixRotationQuaternion(vecRot);

		DirectX::XMFLOAT4X4 rotMat;
		DirectX::XMStoreFloat4x4(&rotMat, matRotMat);

		return rotMat;
	}

	// Ȯ���� ����.
	DirectX::XMFLOAT4X4 Transform::GetLocalRotationMatrix()
	{
		if (!HasParent())
			return GetRotationMatrix();

		auto parentRotMat = GetParent()->GetRotationMatrix();
		auto matParentRotMat = DirectX::XMLoadFloat4x4(&parentRotMat);
		auto rotMat = GetRotationMatrix();
		auto matRotMat = DirectX::XMLoadFloat4x4(&rotMat);

		auto matLocalRotMat = matRotMat * matParentRotMat;

		DirectX::XMFLOAT4X4 localRotMat;
		DirectX::XMStoreFloat4x4(&localRotMat, matLocalRotMat);

		return localRotMat;
	}

	// �ϴ� ȸ�� ��ĸ� ���� �� ��ġ���� ���ߴ�.
	// ���� Ȯ���� �ʿ��ϴ�.
	void Transform::Translate(DirectX::XMFLOAT3 translation, eSpace relativeTo)
	{
		auto vecTranslation = DirectX::XMLoadFloat3(&translation);

		auto mat = relativeTo == eSpace::World ? GetRotationMatrix() : GetLocalRotationMatrix();
		auto vecMat = DirectX::XMLoadFloat4x4(&mat);
		vecTranslation = DirectX::XMVector3Transform(vecTranslation, vecMat);

		auto vecPosition = DirectX::XMLoadFloat3(&m_Position);
		vecPosition = DirectX::XMVectorAdd(vecPosition, vecTranslation);

		DirectX::XMStoreFloat3(&m_Position, vecPosition);
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		Translate(DirectX::XMFLOAT3(x, y, z), relativeTo);
	}

	// �ϴ� ���������� ������ Ȯ���� �ʿ��ϴ�.
	void Transform::Rotate(DirectX::XMFLOAT3 eulerAngles, eSpace relativeTo)
	{
		float radianX = DirectX::XMConvertToRadians(eulerAngles.x);
		float radianY = DirectX::XMConvertToRadians(eulerAngles.y);
		float radianZ = DirectX::XMConvertToRadians(eulerAngles.z);
		auto vecRot = DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ);
		auto matRotMat = DirectX::XMMatrixRotationQuaternion(vecRot);

		auto mat = relativeTo == eSpace::World ? GetRotationMatrix() : GetLocalRotationMatrix();
		auto matMat = DirectX::XMLoadFloat4x4(&mat);

		matRotMat = matRotMat * matMat;

		// ������ �� ����� �ٽ� ���Ϸ� ������ �̾Ƴ� �� m_Rotation�� �����ؾ� �Ѵٴ� ���̴�.
	}

	void Transform::Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo)
	{
		Rotate(DirectX::XMFLOAT3(xAngle, yAngle, zAngle), relativeTo);
	}

	void Transform::Rotate(DirectX::XMFLOAT3 axis, float angle, eSpace relativeTo)
	{
		// axis ���� angle��ŭ ȸ����Ų �� �ٽ� m_Rotation�� ȸ���ϴ� �� ����.
	}

	// ��ġ ��� * ȸ�� ��Ŀ� �� ���͸� ���ϸ� �� �� ����.
	DirectX::XMFLOAT3 Transform::GetForward()
	{
		auto vecPos = DirectX::XMLoadFloat3(&m_Position);
		auto matPosMat = DirectX::XMMatrixTranslationFromVector(vecPos);

		auto rotMat = GetRotationMatrix();
		auto matRotMat = DirectX::XMLoadFloat4x4(&rotMat);

		auto mat = matRotMat * matPosMat;

		DirectX::XMFLOAT3 forward(0.0f, 0.0f, 1.0f);
		auto vecForward = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&forward), mat);
		DirectX::XMStoreFloat3(&forward, vecForward);

		return forward;
	}

	DirectX::XMFLOAT3 Transform::GetUp()
	{
		auto vecPos = DirectX::XMLoadFloat3(&m_Position);
		auto matPosMat = DirectX::XMMatrixTranslationFromVector(vecPos);

		auto rotMat = GetRotationMatrix();
		auto matRotMat = DirectX::XMLoadFloat4x4(&rotMat);

		auto mat = matRotMat * matPosMat;

		DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
		auto vecUp = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&up), mat);
		DirectX::XMStoreFloat3(&up, vecUp);

		return up;
	}

	DirectX::XMFLOAT3 Transform::GetRight()
	{
		auto vecPos = DirectX::XMLoadFloat3(&m_Position);
		auto matPosMat = DirectX::XMMatrixTranslationFromVector(vecPos);

		auto rotMat = GetRotationMatrix();
		auto matRotMat = DirectX::XMLoadFloat4x4(&rotMat);

		auto mat = matRotMat * matPosMat;

		DirectX::XMFLOAT3 right(1.0f, 0.0f, 0.0f);
		auto vecRight = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&right), mat);
		DirectX::XMStoreFloat3(&right, vecRight);

		return right;
	}

	void Transform::LookAt(Transform target, DirectX::XMFLOAT3 worldUp)
	{
		// ȸ�����Ѽ� target�� ������ġ�� ���� ���ͷ� �Ѵ�.
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

	// ���޹��� Parent�� �ڽŰ� ���Ƶ� true
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

	// �ڽ��� �ڽ��� ��� ����
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