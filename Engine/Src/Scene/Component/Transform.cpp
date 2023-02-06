#include "divepch.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"
#include "Math/Math.h"

using namespace DirectX;

namespace Dive
{
	Transform::Transform(Context* pContext)
		: Component(pContext),
		m_pParent(nullptr)
	{
		m_LocalPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LocalScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

		XMStoreFloat4x4(&m_Matrix, XMMatrixIdentity());
	}

	Transform::~Transform()
	{
		DV_LOG_ENGINE_TRACE("Transform �Ҹ� �Ϸ�({0:d}, {1:s})", m_ID, m_pGameObject->GetName());
	}

	void Transform::Clear()
	{
		SetPosition(0.0f, 0.0f, 0.0f);
		SetRotation(0.0f, 0.0f, 0.0f);
		SetScale(1.0f, 1.0f, 1.0f);
	}

	DirectX::XMFLOAT3 Transform::GetPosition()// const
	{
		GetMatrix();
		return *((XMFLOAT3*)&m_Matrix._41);
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& pos)
	{
		XMFLOAT3 newPos = pos;			   
		XMFLOAT3 curPos = GetPosition();
		if ((curPos.x == newPos.x) && (curPos.y == newPos.y) && (curPos.z == newPos.z))
			return;

		if (m_pParent)
		{
			XMVECTOR vecPos = XMVector3Transform(
				XMLoadFloat3(&newPos), 
				XMMatrixInverse(nullptr, m_pParent->GetMatrix())
			);

			XMStoreFloat3(&newPos, vecPos);
		}

		SetLocalPosition(newPos);
	}

	void Transform::SetPosition(float xPos, float yPos, float zPos)
	{
		SetPosition(XMFLOAT3(xPos, yPos, zPos));
	}

	void Transform::SetLocalPosition(float xPos, float yPos, float zPos)
	{
		SetLocalPosition(XMFLOAT3(xPos, yPos, zPos));
	}

	DirectX::XMFLOAT3 Transform::GetRotation() const
	{
		return Math::QuaternionToEularDegrees(GetRotationQuaternion());
	}

	DirectX::XMFLOAT4 Transform::GetRotationQuaternion() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&m_Matrix));

		XMFLOAT4 outRot;
		XMStoreFloat4(&outRot, rot);

		return outRot;
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotation() const
	{
		return Math::QuaternionToEularDegrees(GetLocalRotationQuaternion());
	}

	void Transform::SetRotation(const DirectX::XMFLOAT3& angle)
	{
		float radianX = XMConvertToRadians(angle.x);
		float radianY = XMConvertToRadians(angle.y);
		float radianZ = XMConvertToRadians(angle.z);

		XMFLOAT4 rotQuat;
		XMStoreFloat4(&rotQuat, XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));

		SetRotation(rotQuat);
	}

	void Transform::SetRotation(float xAngle, float yAngle, float zAngle)
	{
		SetRotation(XMFLOAT3(xAngle, yAngle, zAngle));
	}

	void Transform::SetRotation(const DirectX::XMFLOAT4& quaternion)
	{
		XMFLOAT4 newRotQuat = quaternion;
		XMFLOAT4 curRotQuat = GetRotationQuaternion();
		if ((curRotQuat.x == newRotQuat.x) && (curRotQuat.y == newRotQuat.y) &&
			(curRotQuat.z == newRotQuat.z) && (curRotQuat.w == newRotQuat.w))
			return;

		if (m_pParent)
		{
			XMVECTOR newRotQuatVector = XMLoadFloat4(&newRotQuat);
			XMFLOAT4 parentQuat = m_pParent->GetRotationQuaternion();
			XMVECTOR parentQuatVector = XMLoadFloat4(&parentQuat);

			newRotQuatVector = XMQuaternionMultiply(newRotQuatVector, XMQuaternionInverse(parentQuatVector));
			XMStoreFloat4(&newRotQuat, newRotQuatVector);
		}

		SetLocalRotation(newRotQuat);
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT3& angle)
	{
		float radianX = XMConvertToRadians(angle.x);
		float radianY = XMConvertToRadians(angle.y);
		float radianZ = XMConvertToRadians(angle.z);

		XMStoreFloat4(&m_LocalRotation, XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetLocalRotation(float xAngle, float yAngle, float zAngle)
	{
		SetLocalRotation(XMFLOAT3(xAngle, yAngle, zAngle));
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&m_Matrix));

		XMFLOAT3 outScale;
		XMStoreFloat3(&outScale, scale);

		return outScale;
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		XMFLOAT3 newScale = scale;
		XMFLOAT3 currentScale = GetScale();
		if ((newScale.x == currentScale.x) && (newScale.y == currentScale.y) && (newScale.z == currentScale.z))
			return;

		if (m_pParent)
		{
			XMFLOAT3 parentScale = m_pParent->GetScale();
			newScale.x /= parentScale.x;
			newScale.y /= parentScale.y;
			newScale.z /= parentScale.z;
		}

		SetLocalScale(newScale);
	}

	void Transform::SetScale(float xScale, float yScale, float zScale)
	{
		SetScale(XMFLOAT3(xScale, yScale, zScale));
	}

	void Transform::SetLocalScale(float xScale, float yScale, float zScale)
	{
		SetLocalScale(XMFLOAT3(xScale, yScale, zScale));
	}

	XMMATRIX Transform::GetMatrix()
	{
		XMMATRIX mat = GetLocalMatrix();
		if (m_pParent)
			mat *= m_pParent->GetMatrix();

		XMStoreFloat4x4(&m_Matrix, mat);
	
		return mat;
	}

	// �� �ָ��ϴ�. ���� �Լ� ����� �����ߴ�. ���� �Լ��� �ٸ� ��� ���Ǹ� ���ؼ��� �ʿ��� ���δ�.
	// �̸��� ������ �� ������ ���.
	DirectX::XMFLOAT4X4 Transform::GetWorldTransform()
	{
		// ����������� const &�� �����ص� ���� ��������...
		// rastertek�� XMMATRIX�� ��� ������ ����Ѵ�.
		// ������ ���� ������ ��� ���� ����� ��õ���� �ʴ´�...
		GetMatrix();
		return m_Matrix;
	}

	XMMATRIX Transform::GetLocalMatrix() const
	{
		auto vecLocalScale = XMLoadFloat3(&m_LocalScale);
		auto vecLocalRotation = XMLoadFloat4(&m_LocalRotation);
		auto vecLocalTranslation = XMLoadFloat3(&m_LocalPosition);

		return XMMatrixScalingFromVector(vecLocalScale) * XMMatrixRotationQuaternion(vecLocalRotation)
			* XMMatrixTranslationFromVector(vecLocalTranslation);
	}

	void Transform::SetLocalMatrix(const DirectX::XMFLOAT4X4& mat)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&m_Matrix));

		XMStoreFloat3(&m_LocalPosition, pos);
		XMStoreFloat4(&m_LocalRotation, rot);
		XMStoreFloat3(&m_LocalScale, scale);
	}

	void Transform::Translate(DirectX::XMFLOAT3 translation, eSpace relativeTo)
	{
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
	}

	void Transform::Translate(DirectX::XMFLOAT3 translation, Transform relativeTo)
	{
	}

	void Transform::Translate(float x, float y, float z, Transform relativeTo)
	{
	}

	// 1. ���� SetXXX�� Matrix�� �������� �ʴ´�.
	// 2. ���� ���͸� ���� ��ġ�� ��������� ����ؾ� �Ѵ�.
	DirectX::XMFLOAT3 Transform::GetForward()// const
	{
		GetMatrix();
		auto localRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_LocalRotation));
		auto forwardVector = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), localRotationMatrix);
		forwardVector = DirectX::XMVector3Normalize(forwardVector);

		DirectX::XMFLOAT3 forward;
		DirectX::XMStoreFloat3(&forward, forwardVector);

		return forward;
	}

	DirectX::XMFLOAT3 Transform::GetUp()// const
	{
		GetMatrix();
		auto localRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_LocalRotation));
		auto upVector = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), localRotationMatrix);
		upVector = DirectX::XMVector3Normalize(upVector);

		DirectX::XMFLOAT3 up;
		DirectX::XMStoreFloat3(&up, upVector);

		return up;
	}

	DirectX::XMFLOAT3 Transform::GetRight()// const
	{
		GetMatrix();
		auto localRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_LocalRotation));
		auto rightVector = DirectX::XMVector3Transform(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), localRotationMatrix);
		rightVector = DirectX::XMVector3Normalize(rightVector);

		DirectX::XMFLOAT3 right;
		DirectX::XMStoreFloat3(&right, rightVector);

		return right;
	}

	void Transform::SetParent(Transform* pParent)
	{
		if (pParent)
		{
			if (pParent->GetID() == this->GetID())
				return;

			if (m_pParent)
			{
				if (m_pParent->GetID() == pParent->GetID())
					return;

				// �ڽ��� �θ�� ���� �� ����.
				if (pParent->IsChildOf(this))
					return;
				else
				{
					auto it = m_pParent->m_Children.begin();
					for (it; it != m_pParent->m_Children.end(); ++it)
					{
						if ((*it)->GetID() == this->GetID())
						{
							m_pParent->m_Children.erase(it);
							break;
						}
					}

					m_pParent = pParent;
					pParent->m_Children.emplace_back(this);
				}
			}
			else
			{
				m_pParent = pParent;
				pParent->m_Children.emplace_back(this);
			}
		}
		else
		{
			if(m_pParent)
			{
				auto it = m_pParent->m_Children.begin();
				for (it; it != m_pParent->m_Children.end();++it)
				{
					if ((*it)->GetID() == this->GetID())
					{
						m_pParent->m_Children.erase(it);
						break;
					}
				}
				m_pParent = nullptr;
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
		if(m_Children.empty() || static_cast<uint32_t>(m_Children.size()) < index)
			return nullptr;

		return m_Children[index];
	}
	
	bool Transform::IsChildOf(Transform* pParent) const
	{
		if(!pParent)
			return false;

		if (this->GetID() == pParent->GetID())
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

	std::vector<Transform*> Transform::GetChildren() const
	{
		return m_Children;
	}
}