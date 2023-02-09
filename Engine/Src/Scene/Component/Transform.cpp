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

		XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_Matrix, XMMatrixIdentity());
	}

	Transform::~Transform()
	{
		DV_LOG_ENGINE_TRACE("Transform 소멸 완료({0:d}, {1:s})", m_ID, m_pGameObject->GetName());
	}

	void Transform::Clear()
	{
		SetPosition(0.0f, 0.0f, 0.0f);
		SetRotation(0.0f, 0.0f, 0.0f);
		SetScale(1.0f, 1.0f, 1.0f);
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		return *((XMFLOAT3*)&m_Matrix._41);
	}

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		return XMLoadFloat3(((XMFLOAT3*)&m_Matrix._41));
	}

	DirectX::XMVECTOR Transform::GetLocalPositionVector() const
	{
		return XMLoadFloat3(&m_LocalPosition);
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPositionVector(XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& pos)
	{
		SetPositionVector(XMLoadFloat3(&pos));
	}

	void Transform::SetPositionVector(const DirectX::XMVECTOR& pos)
	{
		if (XMVector3Equal(GetPositionVector(), pos))
			return;

		SetLocalPositionVector(
			HasParent() ? XMVector3Transform(pos, XMMatrixInverse(nullptr, m_pParent->GetMatrix())) : pos);
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPositionVector(XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& pos)
	{
		SetLocalPositionVector(XMLoadFloat3(&pos));
	}

	void Transform::SetLocalPositionVector(const DirectX::XMVECTOR& pos)
	{
		if (XMVector3Equal(GetLocalPositionVector(), pos))
			return;

		XMStoreFloat3(&m_LocalPosition, pos);

		updateTransform();
	}

	DirectX::XMFLOAT3 Transform::GetRotation() const
	{
		return Math::QuaternionToEularDegrees(GetRotationQuaternion());
	}

	DirectX::XMFLOAT4 Transform::GetRotationQuaternion() const
	{
		XMFLOAT4 rot;
		XMStoreFloat4(&rot, GetRotationQuaternionVector());

		return rot;
	}

	DirectX::XMVECTOR Transform::GetRotationQuaternionVector() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&m_Matrix));

		return rot;
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotation() const
	{
		return Math::QuaternionToEularDegrees(GetLocalRotationQuaternion());
	}

	DirectX::XMVECTOR Transform::GetLocalRotationQuaternionVector() const
	{
		return XMLoadFloat4(&m_LocalRotation);
	}

	void Transform::SetRotation(float xAngle, float yAngle, float zAngle)
	{
		float radianX = XMConvertToRadians(xAngle);
		float radianY = XMConvertToRadians(yAngle);
		float radianZ = XMConvertToRadians(zAngle);

		SetRotationQuaternionVector(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetRotation(const DirectX::XMFLOAT3& angle)
	{
		float radianX = XMConvertToRadians(angle.x);
		float radianY = XMConvertToRadians(angle.y);
		float radianZ = XMConvertToRadians(angle.z);

		SetRotationQuaternionVector(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetRotationQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotationQuaternionVector(XMLoadFloat4(&quaternion));
	}

	void Transform::SetRotationQuaternionVector(const DirectX::XMVECTOR& quaternion)
	{
		if (XMVector4Equal(GetRotationQuaternionVector(), quaternion))
			return;

		SetLocalRotationQuaternionVector(
			HasParent() ? XMQuaternionMultiply(quaternion, XMQuaternionInverse(m_pParent->GetRotationQuaternionVector())) : quaternion);
	}

	void Transform::SetLocalRotation(float xAngle, float yAngle, float zAngle)
	{
		float radianX = XMConvertToRadians(xAngle);
		float radianY = XMConvertToRadians(yAngle);
		float radianZ = XMConvertToRadians(zAngle);

		SetLocalRotationQuaternionVector(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT3& angle)
	{
		float radianX = XMConvertToRadians(angle.x);
		float radianY = XMConvertToRadians(angle.y);
		float radianZ = XMConvertToRadians(angle.z);

		SetLocalRotationQuaternionVector(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetLocalRotationQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotationQuaternionVector(XMLoadFloat4(&quaternion));
	}

	void Transform::SetLocalRotationQuaternionVector(const DirectX::XMVECTOR& quaternion)
	{
		if (XMVector4Equal(GetLocalRotationQuaternionVector(), quaternion))
			return;

		XMStoreFloat4(&m_LocalRotation, quaternion);

		updateTransform();
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, GetScaleVector());

		return scale;
	}

	DirectX::XMVECTOR Transform::GetScaleVector() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, GetMatrix());

		return scale;
	}

	DirectX::XMVECTOR Transform::GetLocalScaleVector() const
	{
		return XMLoadFloat3(&m_LocalScale);
	}

	void Transform::SetScale(float x, float y, float z)
	{
		if ((x == 0.0f) || (y == 0.0f) || (z == 0.0f))
			return;

		SetScaleVector(XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		if ((scale.x == 0.0f) || (scale.y == 0.0f) || (scale.z == 0.0f))
			return;

		SetScaleVector(XMLoadFloat3(&scale));
	}

	void Transform::SetScaleVector(const DirectX::XMVECTOR& scale)
	{
		if (XMVector3Equal(GetScaleVector(), scale))
			return;

		SetLocalScaleVector(HasParent() ? XMVectorDivide(scale, m_pParent->GetScaleVector()) : scale);
	}

	void Transform::SetLocalScale(float x, float y, float z)
	{
		if ((x == 0.0f) || (y == 0.0f) || (z == 0.0f))
			return;

		SetLocalScaleVector(XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		if ((scale.x == 0.0f) || (scale.y == 0.0f) || (scale.z == 0.0f))
			return;

		SetLocalScaleVector(XMLoadFloat3(&scale));
	}

	void Transform::SetLocalScaleVector(const DirectX::XMVECTOR& scale)
	{
		if (XMVector3Equal(GetLocalScaleVector(), scale))
			return;

		XMFLOAT3 tempScale;
		XMStoreFloat3(&tempScale, scale);

		if ((tempScale.x == 0.0f) || (tempScale.y == 0.0f) || (tempScale.z == 0.0f))
			return;

		m_LocalScale = tempScale;

		updateTransform();
	}

	XMMATRIX Transform::GetMatrix() const
	{
		return XMLoadFloat4x4(&m_Matrix);
	}

	XMMATRIX Transform::GetLocalMatrix() const
	{
		return XMLoadFloat4x4(&m_LocalMatrix);
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		TranslateVector(XMVectorSet(x, y, z, 1.0f), relativeTo);
	}

	void Transform::Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo)
	{
		TranslateVector(XMLoadFloat3(&translation), relativeTo);
	}

	void Transform::TranslateVector(const DirectX::XMVECTOR& translation, eSpace relativeTo)
	{
		if (relativeTo == eSpace::World)
			SetPositionVector(XMVector3Transform(translation, GetMatrix()));
		
		else
			SetLocalPositionVector(XMVector3Transform(translation, GetLocalMatrix()));
		
	}

	void Transform::Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo)
	{
		float radianX = XMConvertToRadians(xAngle);
		float radianY = XMConvertToRadians(yAngle);
		float radianZ = XMConvertToRadians(zAngle);

		RotateQuaternionVector(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ), relativeTo);
	}

	void Transform::Rotate(const DirectX::XMFLOAT3& eulerAngle, eSpace relativeTo)
	{
		float radianX = XMConvertToRadians(eulerAngle.x);
		float radianY = XMConvertToRadians(eulerAngle.y);
		float radianZ = XMConvertToRadians(eulerAngle.z);

		RotateQuaternionVector(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ), relativeTo);
	}

	void Transform::RotateQuaternion(const DirectX::XMFLOAT4& quaternion, eSpace relativeTo)
	{
		RotateQuaternionVector(XMLoadFloat4(&quaternion), relativeTo);
	}

	void Transform::RotateQuaternionVector(const DirectX::XMVECTOR& quaternion, eSpace relativeTo)
	{
		if (relativeTo == eSpace::World)
			SetRotationQuaternionVector(XMVector4Normalize(XMQuaternionMultiply(quaternion, GetRotationQuaternionVector())));
		else
			SetLocalRotationQuaternionVector(XMVector4Normalize(XMQuaternionMultiply(quaternion, GetLocalRotationQuaternionVector())));
	}

	DirectX::XMFLOAT3 Transform::GetForward() const
	{
		auto localRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_LocalRotation));
		auto forwardVector = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), localRotationMatrix);
		forwardVector = DirectX::XMVector3Normalize(forwardVector);

		DirectX::XMFLOAT3 forward;
		DirectX::XMStoreFloat3(&forward, forwardVector);

		return forward;
	}

	DirectX::XMFLOAT3 Transform::GetUp() const
	{
		auto localRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_LocalRotation));
		auto upVector = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), localRotationMatrix);
		upVector = DirectX::XMVector3Normalize(upVector);

		DirectX::XMFLOAT3 up;
		DirectX::XMStoreFloat3(&up, upVector);

		return up;
	}

	DirectX::XMFLOAT3 Transform::GetRight() const
	{
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

				// 자식을 부모로 삼을 수 없다.
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

	void Transform::updateTransform()
	{
		auto localMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&m_LocalScale)) *
			XMMatrixRotationQuaternion(XMLoadFloat4(&m_LocalRotation)) *
			XMMatrixTranslationFromVector(XMLoadFloat3(&m_LocalPosition));
		XMStoreFloat4x4(&m_LocalMatrix, localMatrix);

		if (m_pParent)
		{
			auto matrix = localMatrix * m_pParent->GetMatrix();
			XMStoreFloat4x4(&m_Matrix, matrix);
		}
		else
			m_Matrix = m_LocalMatrix;

		for (auto pChild : m_Children)
			pChild->updateTransform();
	}
}