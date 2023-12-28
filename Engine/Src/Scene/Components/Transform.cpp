#include "divepch.h"
#include "Transform.h"
#include "Core/CoreDefs.h"
#include "Math/Math.h"

using namespace DirectX;

namespace Dive
{
	Transform::Transform(GameObject* pGameObject)
		: Component(pGameObject)
		, m_pParent(nullptr)
	{
		m_LocalPosition = { 0.0f, 0.0f, 0.0f };
		m_LocalRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_LocalScale = { 1.0f, 1.0f, 1.0f };

		XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	}

	void Transform::Update()
	{
		updateTransform();
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		return { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 };
	}

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		auto pos = GetPosition();
		return XMLoadFloat3(&pos);
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& pos)
	{
		SetPosition(XMLoadFloat3(&pos));
	}

	void Transform::SetPosition(float posX, float posY, float posZ)
	{
		SetPosition(XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void Transform::SetPosition(const DirectX::XMVECTOR& pos)
	{
		auto curPos = GetPositionVector();
		if (!XMVector3Equal(curPos, pos))
		{
			XMVECTOR localPos = pos;
			if (HasParent())
				localPos = XMVector3Transform(localPos, XMMatrixInverse(nullptr, GetParent()->GetWorldMatrix()));

			SetLocalPosition(localPos);
		}
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& pos)
	{
		if(!XMVector3Equal(GetLocalPositionVector(), XMLoadFloat3(&pos)))
		{
			m_LocalPosition = pos;
			updateTransform();
		}
	}

	void Transform::SetLocalPosition(float posX, float posY, float posZ)
	{
		SetLocalPosition(XMVectorSet(posX, posY, posZ, 1.0f));
	}

	// 부모가 있다면 부모로 좌표로부터의 상대적 위치다.
	// 없다면 월드 좌표와 같다.
	void Transform::SetLocalPosition(const DirectX::XMVECTOR& pos)
	{
		auto localPosVector = XMLoadFloat3(&m_LocalPosition);
		if (!XMVector3Equal(localPosVector, pos))
		{
			XMStoreFloat3(&m_LocalPosition, pos);
			updateTransform();
		}
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
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&m_WorldMatrix));

		return rot;
	}

	DirectX::XMFLOAT3 Transform::GetRotationDegree() const
	{
		return Math::QuaternionToDegree(GetRotationQuaternion());
	}

	DirectX::XMFLOAT3 Transform::GetLocalRotationDegree() const
	{
		return Math::QuaternionToDegree(m_LocalRotation);
	}

	void Transform::SetRotation(float degreeX, float degreeY, float degreeZ)
	{
		float angleX = XMConvertToRadians(degreeX);
		float angleY = XMConvertToRadians(degreeY);
		float angleZ = XMConvertToRadians(degreeZ);

		SetRotation(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ));
	}

	void Transform::SetRotation(const DirectX::XMFLOAT3& degree)
	{
		float angleX = XMConvertToRadians(degree.x);
		float angleY = XMConvertToRadians(degree.y);
		float angleZ = XMConvertToRadians(degree.z);

		SetRotation(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ));
	}

	void Transform::SetRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotation(XMLoadFloat4(&quaternion));
	}

	void Transform::SetRotation(const DirectX::XMVECTOR& quaternion)
	{
		auto curRotation = GetRotationQuaternionVector();
		if (!XMVector4Equal(curRotation, quaternion))
		{
			SetLocalRotation(
				HasParent() ? XMQuaternionMultiply(quaternion, XMQuaternionInverse(GetParent()->GetRotationQuaternionVector())) : quaternion);
		}
	}

	void Transform::SetLocalRotation(float degreeX, float degreeY, float degreeZ)
	{
		float angleX = XMConvertToRadians(degreeX);
		float angleY = XMConvertToRadians(degreeY);
		float angleZ = XMConvertToRadians(degreeZ);

		SetLocalRotation(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ));
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT3& degree)
	{
		float angleX = XMConvertToRadians(degree.x);
		float angleY = XMConvertToRadians(degree.y);
		float angleZ = XMConvertToRadians(degree.z);

		SetLocalRotation(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ));
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotation(XMLoadFloat4(&quaternion));
	}

	void Transform::SetLocalRotation(const DirectX::XMVECTOR& quaternion)
	{
		auto curLocalRotation = GetLocalRotationQuaternionVector();
		if (!XMVector4Equal(curLocalRotation, quaternion))
		{
			XMStoreFloat4(&m_LocalRotation, quaternion);
			updateTransform();
		}
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
		XMMatrixDecompose(&scale, &rot, &pos, GetWorldMatrix());

		return scale;
	}

	void Transform::SetScale(float scaleX, float scaleY, float scaleZ)
	{
		SetScale(XMVectorSet(scaleX, scaleY, scaleZ, 1.0f));
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		SetScale(XMLoadFloat3(&scale));
	}

	void Transform::SetScale(const DirectX::XMVECTOR& scale)
	{
		auto curScale = GetScaleVector();
		if (!XMVector3Equal(curScale, scale))
		{
			auto localScale = GetLocalScaleVector();
			SetLocalScale(HasParent() ? XMVectorDivide(localScale, GetParent()->GetScaleVector()) : localScale);
		}
	}

	void Transform::SetLocalScale(float scaleX, float scaleY, float scaleZ)
	{
		SetLocalScale(XMFLOAT3(scaleX, scaleY, scaleZ));
	}

	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		if (!XMVector3Equal(GetLocalScaleVector(), XMLoadFloat3(&scale)))
		{
			m_LocalScale = scale;
			updateTransform();
		}
	}

	void Transform::SetLocalScale(const DirectX::XMVECTOR& scale)
	{
		if (!XMVector3Equal(GetLocalScaleVector(), scale))
		{
			XMStoreFloat3(&m_LocalScale, scale);
			updateTransform();
		}
	}

	// 로컬 좌표 위의 한 점을 월드 좌표로...
	DirectX::XMMATRIX Transform::GetLocalToWorldMatrix() const
	{
		return HasParent()? GetParent()->GetWorldMatrix() : XMMatrixIdentity();
	}

	// 월드 좌표 위의 한 점을 로컬 좌표로...
	DirectX::XMMATRIX Transform::GetWorldToLocalMatrix() const
	{
		return GetLocalMatrix();
	}

	DirectX::XMMATRIX Transform::GetPositionMatrix() const
	{
		return XMMatrixTranslationFromVector(GetPositionVector());
	}

	DirectX::XMMATRIX Transform::GetRotationMatrix() const
	{
		return XMMatrixRotationQuaternion(GetRotationQuaternionVector());
	}

	DirectX::XMMATRIX Transform::GetScaleMatrix() const
	{
		return XMMatrixScalingFromVector(GetScaleVector());
	}

	DirectX::XMMATRIX Transform::GetLocalPositionMatrix() const
	{
		return XMMatrixTranslationFromVector(XMLoadFloat3(&m_LocalPosition));
	}

	DirectX::XMMATRIX Transform::GetLocalRotationMatrix() const
	{
		return XMMatrixRotationQuaternion(XMLoadFloat4(&m_LocalRotation));
	}

	DirectX::XMMATRIX Transform::GetLocalScaleMatrix() const
	{
		return XMMatrixScalingFromVector(XMLoadFloat3(&m_LocalScale));
	}

	DirectX::XMMATRIX Transform::GetWorldMatrix() const
	{
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	DirectX::XMMATRIX Transform::GetLocalMatrix() const
	{
		return XMLoadFloat4x4(&m_LocalMatrix);
	}

	void Transform::SetWorldMatrix(const DirectX::XMFLOAT4X4& mat)
	{
		SetWorldMatrix(XMLoadFloat4x4(&mat));
	}

	void Transform::SetWorldMatrix(const DirectX::XMMATRIX& mat)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, mat);

		SetPosition(pos);
		SetRotation(rot);
		SetScale(scale);
	}

	void Transform::SetLocalMatrix(const DirectX::XMFLOAT4X4& mat)
	{
		SetLocalMatrix(XMLoadFloat4x4(&mat));
	}

	void Transform::SetLocalMatrix(const DirectX::XMMATRIX& mat)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, mat);

		SetLocalPosition(pos);
		SetLocalRotation(rot);
		SetLocalScale(scale);
	}

	bool Transform::HasParent() const
	{
		return m_pParent;
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
			if (m_pParent)
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
		if (m_Children.empty() || static_cast<uint32_t>(m_Children.size()) < index)
			return nullptr;

		return m_Children[index];
	}

	bool Transform::IsChildOf(Transform* pParent) const
	{
		DV_CORE_ASSERT(pParent);

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

	Transform* Transform::FindByName(const std::string& name)
	{
		if (GetName() == name)
			return this;

		for (uint32_t i = 0; i < GetChildCount(); ++i)
		{
			auto pFound = m_Children[i]->FindByName(name);
			if (pFound)
				return pFound;
		}

		return nullptr;
	}

	void Transform::updateTransform()
	{
		const auto localMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&m_LocalScale)) *
			XMMatrixRotationQuaternion(XMLoadFloat4(&m_LocalRotation)) *
			XMMatrixTranslationFromVector(XMLoadFloat3(&m_LocalPosition));
		XMStoreFloat4x4(&m_LocalMatrix, localMatrix);

		if (m_pParent)
		{
			const auto matrix = XMMatrixMultiply(localMatrix, m_pParent->GetWorldMatrix());
			XMStoreFloat4x4(&m_WorldMatrix, matrix);
		}
		else
			m_WorldMatrix = m_LocalMatrix;

		for (auto pChild : m_Children)
			pChild->updateTransform();
	}

	DirectX::XMFLOAT3 Transform::GetForward() const
	{
		XMFLOAT3 forward;
		XMStoreFloat3(&forward, GetForwardVector());
		
		return forward;
	}
	
	DirectX::XMVECTOR Transform::GetForwardVector() const
	{
		return XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), GetRotationMatrix());
	}

	DirectX::XMFLOAT3 Transform::GetUpward() const
	{
		XMFLOAT3 upward;
		XMStoreFloat3(&upward, GetUpwardVector());

		return upward;
	}

	DirectX::XMVECTOR Transform::GetUpwardVector() const
	{
		return XMVector3Transform(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), GetRotationMatrix());
	}

	DirectX::XMFLOAT3 Transform::GetRightward() const
	{
		XMFLOAT3 rightward;
		XMStoreFloat3(&rightward, GetRightwardVector());

		return rightward;
	}

	DirectX::XMVECTOR Transform::GetRightwardVector() const
	{
		return XMVector3Transform(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), GetRotationMatrix());
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		Translate(XMVectorSet(x, y, z, 1.0f), relativeTo);
	}

	void Transform::Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo)
	{
		Translate(XMLoadFloat3(&translation), relativeTo);
	}

	// self space 확인 필요
	void Transform::Translate(const DirectX::XMVECTOR& translation, eSpace relativeTo)
	{
		XMVECTOR newPos;

		if (relativeTo == eSpace::World)
		{
			newPos = XMVectorAdd(GetPositionVector(), translation);
			SetPosition(newPos);
		}
		else
		{
			auto localTranslation = XMVector3Transform(translation, GetLocalRotationMatrix());
			newPos = XMVectorAdd(GetLocalPositionVector(), localTranslation);
			SetLocalPosition(newPos);
		}
	}

	void Transform::Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo)
	{
		float angleX = XMConvertToRadians(degreeX);
		float angleY = XMConvertToRadians(degreeY);
		float angleZ = XMConvertToRadians(degreeZ);

		Rotate(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ), relativeTo);
	}
	
	void Transform::Rotate(const DirectX::XMFLOAT3& degree, eSpace relativeTo)
	{
		float angleX = XMConvertToRadians(degree.x);
		float angleY = XMConvertToRadians(degree.y);
		float angleZ = XMConvertToRadians(degree.z);

		Rotate(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ), relativeTo);
	}
	
	void Transform::Rotate(const DirectX::XMFLOAT4& delta, eSpace relativeTo) 
	{
		Rotate(XMLoadFloat4(&delta), relativeTo);
	}
	
	void Transform::Rotate(const DirectX::XMVECTOR& delta, eSpace relativeTo)
	{
		// 단순하게 보자면 그냥 스페이스에 맞춰 회전 행렬을 가져온 후 곱하고 새로운 회전으로 저장하면 될 것 같은데...
		// 스파르탄의 경우 로컬회전은 기존 회전이랑 곱한 후 정규화를 하여 다시 회전으로 저장하는데
		// 월드회전의 경우 '로컬회전 * 월드역회전 * 전달받은 회전 * 월드회전'이다.
		if (relativeTo == eSpace::World)
		{

		}
		else
		{
			auto rot = XMQuaternionMultiply(GetLocalRotationQuaternionVector(), delta);
			SetLocalRotation(XMQuaternionNormalize(rot));
		}
	}

	DirectX::XMFLOAT3 Transform::GetLookAt()
	{
		XMFLOAT3 lookAt;
		XMStoreFloat3(&lookAt, GetLookAtVector());
		return lookAt;
	}

	DirectX::XMVECTOR Transform::GetLookAtVector()
	{
		return XMVectorAdd(GetPositionVector(), GetForwardVector());
	}
}