#include "divepch.h"
#include "DvTransform.h"
#include "Core/CoreDefs.h"
#include "Math/Math.h"

using namespace DirectX;

namespace Dive
{
	DvTransform::DvTransform(DvGameObject* pGameObject)
		: DvComponent(pGameObject)
		, m_pParent(nullptr)
	{
		m_LocalPosition = { 0.0f, 0.0f, 0.0f };
		m_LocalRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_LocalScale = { 1.0f, 1.0f, 1.0f };

		XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	}

	void DvTransform::Update()
	{
		updateTransform();
	}

	DirectX::XMFLOAT3 DvTransform::GetPosition() const
	{
		return { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 };
	}

	DirectX::XMVECTOR DvTransform::GetPositionVector() const
	{
		auto pos = GetPosition();
		return XMLoadFloat3(&pos);
	}

	void DvTransform::SetPosition(const DirectX::XMFLOAT3& pos)
	{
		SetPosition(XMLoadFloat3(&pos));
	}

	void DvTransform::SetPosition(float posX, float posY, float posZ)
	{
		SetPosition(XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void DvTransform::SetPosition(const DirectX::XMVECTOR& pos)
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

	void DvTransform::SetLocalPosition(const DirectX::XMFLOAT3& pos)
	{
		if (!XMVector3Equal(GetLocalPositionVector(), XMLoadFloat3(&pos)))
		{
			m_LocalPosition = pos;
			updateTransform();
		}
	}

	void DvTransform::SetLocalPosition(float posX, float posY, float posZ)
	{
		SetLocalPosition(XMVectorSet(posX, posY, posZ, 1.0f));
	}

	// 부모가 있다면 부모로 좌표로부터의 상대적 위치다.
	// 없다면 월드 좌표와 같다.
	void DvTransform::SetLocalPosition(const DirectX::XMVECTOR& pos)
	{
		auto localPosVector = XMLoadFloat3(&m_LocalPosition);
		if (!XMVector3Equal(localPosVector, pos))
		{
			XMStoreFloat3(&m_LocalPosition, pos);
			updateTransform();
		}
	}

	DirectX::XMFLOAT4 DvTransform::GetRotationQuaternion() const
	{
		XMFLOAT4 rot;
		XMStoreFloat4(&rot, GetRotationQuaternionVector());

		return rot;
	}

	DirectX::XMVECTOR DvTransform::GetRotationQuaternionVector() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&m_WorldMatrix));

		return rot;
	}

	DirectX::XMFLOAT3 DvTransform::GetRotationDegree() const
	{
		return Math::QuaternionToDegree(GetRotationQuaternion());
	}

	DirectX::XMFLOAT3 DvTransform::GetLocalRotationDegree() const
	{
		return Math::QuaternionToDegree(m_LocalRotation);
	}

	void DvTransform::SetRotation(float degreeX, float degreeY, float degreeZ)
	{
		float angleX = XMConvertToRadians(degreeX);
		float angleY = XMConvertToRadians(degreeY);
		float angleZ = XMConvertToRadians(degreeZ);

		SetRotation(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ));
	}

	void DvTransform::SetRotation(const DirectX::XMFLOAT3& degree)
	{
		float angleX = XMConvertToRadians(degree.x);
		float angleY = XMConvertToRadians(degree.y);
		float angleZ = XMConvertToRadians(degree.z);

		SetRotation(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ));
	}

	void DvTransform::SetRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotation(XMLoadFloat4(&quaternion));
	}

	void DvTransform::SetRotation(const DirectX::XMVECTOR& quaternion)
	{
		auto curRotation = GetRotationQuaternionVector();
		if (!XMVector4Equal(curRotation, quaternion))
		{
			SetLocalRotation(
				HasParent() ? XMQuaternionMultiply(quaternion, XMQuaternionInverse(GetParent()->GetRotationQuaternionVector())) : quaternion);
		}
	}

	void DvTransform::SetLocalRotation(float degreeX, float degreeY, float degreeZ)
	{
		float angleX = XMConvertToRadians(degreeX);
		float angleY = XMConvertToRadians(degreeY);
		float angleZ = XMConvertToRadians(degreeZ);

		SetLocalRotation(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ));
	}

	void DvTransform::SetLocalRotation(const DirectX::XMFLOAT3& degree)
	{
		float angleX = XMConvertToRadians(degree.x);
		float angleY = XMConvertToRadians(degree.y);
		float angleZ = XMConvertToRadians(degree.z);

		SetLocalRotation(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ));
	}

	void DvTransform::SetLocalRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotation(XMLoadFloat4(&quaternion));
	}

	void DvTransform::SetLocalRotation(const DirectX::XMVECTOR& quaternion)
	{
		auto curLocalRotation = GetLocalRotationQuaternionVector();
		if (!XMVector4Equal(curLocalRotation, quaternion))
		{
			XMStoreFloat4(&m_LocalRotation, quaternion);
			updateTransform();
		}
	}

	DirectX::XMFLOAT3 DvTransform::GetScale() const
	{
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, GetScaleVector());

		return scale;
	}

	DirectX::XMVECTOR DvTransform::GetScaleVector() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, GetWorldMatrix());

		return scale;
	}

	void DvTransform::SetScale(float scaleX, float scaleY, float scaleZ)
	{
		SetScale(XMVectorSet(scaleX, scaleY, scaleZ, 1.0f));
	}

	void DvTransform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		SetScale(XMLoadFloat3(&scale));
	}

	void DvTransform::SetScale(const DirectX::XMVECTOR& scale)
	{
		auto curScale = GetScaleVector();
		if (!XMVector3Equal(curScale, scale))
		{
			auto localScale = GetLocalScaleVector();
			SetLocalScale(HasParent() ? XMVectorDivide(localScale, GetParent()->GetScaleVector()) : localScale);
		}
	}

	void DvTransform::SetLocalScale(float scaleX, float scaleY, float scaleZ)
	{
		SetLocalScale(XMFLOAT3(scaleX, scaleY, scaleZ));
	}

	void DvTransform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		if (!XMVector3Equal(GetLocalScaleVector(), XMLoadFloat3(&scale)))
		{
			m_LocalScale = scale;
			updateTransform();
		}
	}

	void DvTransform::SetLocalScale(const DirectX::XMVECTOR& scale)
	{
		if (!XMVector3Equal(GetLocalScaleVector(), scale))
		{
			XMStoreFloat3(&m_LocalScale, scale);
			updateTransform();
		}
	}

	// 로컬 좌표 위의 한 점을 월드 좌표로...
	DirectX::XMMATRIX DvTransform::GetLocalToWorldMatrix() const
	{
		return HasParent() ? GetParent()->GetWorldMatrix() : XMMatrixIdentity();
	}

	// 월드 좌표 위의 한 점을 로컬 좌표로...
	DirectX::XMMATRIX DvTransform::GetWorldToLocalMatrix() const
	{
		return GetLocalMatrix();
	}

	DirectX::XMMATRIX DvTransform::GetPositionMatrix() const
	{
		return XMMatrixTranslationFromVector(GetPositionVector());
	}

	DirectX::XMMATRIX DvTransform::GetRotationMatrix() const
	{
		return XMMatrixRotationQuaternion(GetRotationQuaternionVector());
	}

	DirectX::XMMATRIX DvTransform::GetScaleMatrix() const
	{
		return XMMatrixScalingFromVector(GetScaleVector());
	}

	DirectX::XMMATRIX DvTransform::GetLocalPositionMatrix() const
	{
		return XMMatrixTranslationFromVector(XMLoadFloat3(&m_LocalPosition));
	}

	DirectX::XMMATRIX DvTransform::GetLocalRotationMatrix() const
	{
		return XMMatrixRotationQuaternion(XMLoadFloat4(&m_LocalRotation));
	}

	DirectX::XMMATRIX DvTransform::GetLocalScaleMatrix() const
	{
		return XMMatrixScalingFromVector(XMLoadFloat3(&m_LocalScale));
	}

	DirectX::XMMATRIX DvTransform::GetWorldMatrix() const
	{
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	DirectX::XMMATRIX DvTransform::GetLocalMatrix() const
	{
		return XMLoadFloat4x4(&m_LocalMatrix);
	}

	void DvTransform::SetWorldMatrix(const DirectX::XMFLOAT4X4& mat)
	{
		SetWorldMatrix(XMLoadFloat4x4(&mat));
	}

	void DvTransform::SetWorldMatrix(const DirectX::XMMATRIX& mat)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, mat);

		SetPosition(pos);
		SetRotation(rot);
		SetScale(scale);
	}

	void DvTransform::SetLocalMatrix(const DirectX::XMFLOAT4X4& mat)
	{
		SetLocalMatrix(XMLoadFloat4x4(&mat));
	}

	void DvTransform::SetLocalMatrix(const DirectX::XMMATRIX& mat)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, mat);

		SetLocalPosition(pos);
		SetLocalRotation(rot);
		SetLocalScale(scale);
	}

	bool DvTransform::HasParent() const
	{
		return m_pParent;
	}

	void DvTransform::SetParent(DvTransform* pParent)
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

	DvTransform* DvTransform::GetRoot()
	{
		if (!m_pParent)
			return this;

		return m_pParent->GetRoot();
	}

	DvTransform* DvTransform::GetChild(uint32_t index) const
	{
		if (m_Children.empty() || static_cast<uint32_t>(m_Children.size()) < index)
			return nullptr;

		return m_Children[index];
	}

	bool DvTransform::IsChildOf(DvTransform* pParent) const
	{
		DV_CORE_ASSERT(pParent);

		if (this->GetID() == pParent->GetID())
			return true;

		if (!m_pParent)
			return false;
		else
			return m_pParent->IsChildOf(pParent);
	}

	void DvTransform::DetachChildren()
	{
		for (auto* pChild : m_Children)
		{
			pChild->SetParent(nullptr);
		}
		m_Children.clear();
		m_Children.shrink_to_fit();
	}

	DvTransform* DvTransform::FindByName(const std::string& name)
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

	void DvTransform::updateTransform()
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

	DirectX::XMFLOAT3 DvTransform::GetForward() const
	{
		XMFLOAT3 forward;
		XMStoreFloat3(&forward, GetForwardVector());

		return forward;
	}

	DirectX::XMVECTOR DvTransform::GetForwardVector() const
	{
		return XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), GetRotationMatrix());
	}

	DirectX::XMFLOAT3 DvTransform::GetUpward() const
	{
		XMFLOAT3 upward;
		XMStoreFloat3(&upward, GetUpwardVector());

		return upward;
	}

	DirectX::XMVECTOR DvTransform::GetUpwardVector() const
	{
		return XMVector3Transform(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), GetRotationMatrix());
	}

	DirectX::XMFLOAT3 DvTransform::GetRightward() const
	{
		XMFLOAT3 rightward;
		XMStoreFloat3(&rightward, GetRightwardVector());

		return rightward;
	}

	DirectX::XMVECTOR DvTransform::GetRightwardVector() const
	{
		return XMVector3Transform(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), GetRotationMatrix());
	}

	void DvTransform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		Translate(XMVectorSet(x, y, z, 1.0f), relativeTo);
	}

	void DvTransform::Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo)
	{
		Translate(XMLoadFloat3(&translation), relativeTo);
	}

	// self space 확인 필요
	void DvTransform::Translate(const DirectX::XMVECTOR& translation, eSpace relativeTo)
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

	void DvTransform::Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo)
	{
		float angleX = XMConvertToRadians(degreeX);
		float angleY = XMConvertToRadians(degreeY);
		float angleZ = XMConvertToRadians(degreeZ);

		Rotate(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ), relativeTo);
	}

	void DvTransform::Rotate(const DirectX::XMFLOAT3& degree, eSpace relativeTo)
	{
		float angleX = XMConvertToRadians(degree.x);
		float angleY = XMConvertToRadians(degree.y);
		float angleZ = XMConvertToRadians(degree.z);

		Rotate(XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ), relativeTo);
	}

	void DvTransform::Rotate(const DirectX::XMFLOAT4& delta, eSpace relativeTo)
	{
		Rotate(XMLoadFloat4(&delta), relativeTo);
	}

	void DvTransform::Rotate(const DirectX::XMVECTOR& delta, eSpace relativeTo)
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

	DirectX::XMFLOAT3 DvTransform::GetLookAt()
	{
		XMFLOAT3 lookAt;
		XMStoreFloat3(&lookAt, GetLookAtVector());
		return lookAt;
	}

	DirectX::XMVECTOR DvTransform::GetLookAtVector()
	{
		return XMVectorAdd(GetPositionVector(), GetForwardVector());
	}
}