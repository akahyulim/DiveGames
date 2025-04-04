#include "DivePch.h"
#include "OldTransform.h"
#include "Scene/GameObject.h"
#include "Math/Math.h"
#include "Core/Log.h"

using namespace DirectX;

namespace Dive
{
	OldTransform::OldTransform()
		: Component(nullptr),
		m_pParent(nullptr)
	{
		m_LocalPosition = { 0.0f, 0.0f, 0.0f };
		m_LocalRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_LocalScale = { 1.0f, 1.0f, 1.0f };

		m_LookAt = { 0.0f, 0.0f, 1.0f };

		XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_Matrix, XMMatrixIdentity());
	}

	OldTransform::OldTransform(GameObject* pGameObject)
		: Component(pGameObject),
		m_pParent(nullptr)
	{
		m_LocalPosition = { 0.0f, 0.0f, 0.0f };
		m_LocalRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_LocalScale = { 1.0f, 1.0f, 1.0f };

		m_LookAt = { 0.0f, 0.0f, 1.0f };

		XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_Matrix, XMMatrixIdentity());
	}

	OldTransform::~OldTransform()
	{
	}

	bool OldTransform::LoadFromFile(const std::string& filePath)
	{
		return true;
	}

	bool OldTransform::SaveToFile(const std::string& filePath)
	{
		DV_CORE_INFO("Save OldTransform: {:d}", GetID());

		return true;
	}

	// 좀 애매하다.
	void OldTransform::Update()
	{
		updateTransform();
	}

	void OldTransform::Clear()
	{
		SetPosition(0.0f, 0.0f, 0.0f);
		SetRotation(0.0f, 0.0f, 0.0f);
		SetScale(1.0f, 1.0f, 1.0f);
	}

	DirectX::XMVECTOR OldTransform::GetPosition() const
	{
		return XMVectorSet(m_Matrix._41, m_Matrix._42, m_Matrix._43, 1.0f);
	}

	void OldTransform::GetPosition(DirectX::XMFLOAT3& pos) const
	{
		pos = { m_Matrix._41, m_Matrix._42, m_Matrix._43 };
	}

	DirectX::XMVECTOR OldTransform::GetLocalPosition() const
	{
		return XMLoadFloat3(&m_LocalPosition);
	}

	void OldTransform::GetLocalPosition(DirectX::XMFLOAT3& localPos) const
	{
		localPos = m_LocalPosition;
	}

	void OldTransform::SetPosition(float x, float y, float z)
	{
		SetPosition(XMVectorSet(x, y, z, 1.0f));
	}

	void OldTransform::SetPosition(const DirectX::XMFLOAT3& pos)
	{
		SetPosition(XMLoadFloat3(&pos));
	}

	void OldTransform::SetPosition(const DirectX::XMVECTOR& pos)
	{
		if (XMVector3Equal(GetPosition(), pos))
			return;

		SetLocalPosition(
			HasParent() ? XMVector3Transform(pos, XMMatrixInverse(nullptr, m_pParent->GetMatrix())) : pos);
	}

	void OldTransform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPosition(XMVectorSet(x, y, z, 1.0f));
	}

	void OldTransform::SetLocalPosition(const DirectX::XMFLOAT3& pos)
	{
		SetLocalPosition(XMLoadFloat3(&pos));
	}

	void OldTransform::SetLocalPosition(const DirectX::XMVECTOR& pos)
	{
		if (XMVector3Equal(GetLocalPosition(), pos))
			return;

		XMStoreFloat3(&m_LocalPosition, pos);

		updateTransform();
	}

	DirectX::XMVECTOR OldTransform::GetRotation() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&m_Matrix));

		return rot;
	}

	DirectX::XMVECTOR OldTransform::GetLocalRotation() const
	{
		return XMLoadFloat4(&m_LocalRotation);
	}

	DirectX::XMFLOAT3 OldTransform::GetLocalRotationDegrees() const
	{
		return Math::QuaternionToEularDegrees(m_LocalRotation);
	}

	void OldTransform::SetRotation(float xAngle, float yAngle, float zAngle)
	{
		float radianX = XMConvertToRadians(xAngle);
		float radianY = XMConvertToRadians(yAngle);
		float radianZ = XMConvertToRadians(zAngle);

		SetRotationQuaternion(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void OldTransform::SetRotation(const DirectX::XMFLOAT3& angle)
	{
		float radianX = XMConvertToRadians(angle.x);
		float radianY = XMConvertToRadians(angle.y);
		float radianZ = XMConvertToRadians(angle.z);

		SetRotationQuaternion(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void OldTransform::SetRotationQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotationQuaternion(XMLoadFloat4(&quaternion));
	}

	void OldTransform::SetRotationQuaternion(const DirectX::XMVECTOR& quaternion)
	{
		if (XMVector4Equal(GetRotation(), quaternion))
			return;

		SetLocalRotationQuaternion(
			HasParent() ? XMQuaternionMultiply(quaternion, XMQuaternionInverse(m_pParent->GetRotation())) : quaternion);
	}

	void OldTransform::SetLocalRotation(float xAngle, float yAngle, float zAngle)
	{
		float radianX = XMConvertToRadians(xAngle);
		float radianY = XMConvertToRadians(yAngle);
		float radianZ = XMConvertToRadians(zAngle);

		SetLocalRotationQuaternion(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void OldTransform::SetLocalRotation(const DirectX::XMFLOAT3& angle)
	{
		float radianX = XMConvertToRadians(angle.x);
		float radianY = XMConvertToRadians(angle.y);
		float radianZ = XMConvertToRadians(angle.z);

		SetLocalRotationQuaternion(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void OldTransform::SetLocalRotationQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotationQuaternion(XMLoadFloat4(&quaternion));
	}

	void OldTransform::SetLocalRotationQuaternion(const DirectX::XMVECTOR& quaternion)
	{
		if (XMVector4Equal(GetLocalRotation(), quaternion))
			return;

		XMStoreFloat4(&m_LocalRotation, quaternion);

		updateTransform();
	}

	DirectX::XMVECTOR OldTransform::GetScale() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, GetMatrix());

		return scale;
	}

	DirectX::XMVECTOR OldTransform::GetLocalScale() const
	{
		return XMLoadFloat3(&m_LocalScale);
	}

	void OldTransform::SetScale(float x, float y, float z)
	{
		if ((x == 0.0f) || (y == 0.0f) || (z == 0.0f))
			return;

		SetScale(XMVectorSet(x, y, z, 1.0f));
	}

	void OldTransform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		if ((scale.x == 0.0f) || (scale.y == 0.0f) || (scale.z == 0.0f))
			return;

		SetScale(XMLoadFloat3(&scale));
	}

	void OldTransform::SetScale(const DirectX::XMVECTOR& scale)
	{
		if (XMVector3Equal(GetScale(), scale))
			return;

		SetLocalScale(HasParent() ? XMVectorDivide(scale, m_pParent->GetScale()) : scale);
	}

	void OldTransform::SetLocalScale(float x, float y, float z)
	{
		if ((x == 0.0f) || (y == 0.0f) || (z == 0.0f))
			return;

		SetLocalScale(XMVectorSet(x, y, z, 1.0f));
	}

	void OldTransform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		if ((scale.x == 0.0f) || (scale.y == 0.0f) || (scale.z == 0.0f))
			return;

		SetLocalScale(XMLoadFloat3(&scale));
	}

	void OldTransform::SetLocalScale(const DirectX::XMVECTOR& scale)
	{
		if (XMVector3Equal(GetLocalScale(), scale))
			return;

		XMFLOAT3 tempScale;
		XMStoreFloat3(&tempScale, scale);

		if ((tempScale.x == 0.0f) || (tempScale.y == 0.0f) || (tempScale.z == 0.0f))
			return;

		m_LocalScale = tempScale;

		updateTransform();
	}

	XMMATRIX OldTransform::GetMatrix() const
	{
		return XMLoadFloat4x4(&m_Matrix);
	}

	void OldTransform::SetMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&matrix));

		SetPosition(pos);
		SetRotationQuaternion(rot);
		SetScale(scale);
	}

	XMMATRIX OldTransform::GetLocalMatrix() const
	{
		return XMLoadFloat4x4(&m_LocalMatrix);
	}

	void OldTransform::SetLocalMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&matrix));

		SetLocalPosition(pos);
		SetLocalRotationQuaternion(rot);
		SetLocalScale(scale);
	}

	void OldTransform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		Translate(XMVectorSet(x, y, z, 0.0f), relativeTo);
	}

	void OldTransform::Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo)
	{
		Translate(XMLoadFloat3(&translation), relativeTo);
	}

	void OldTransform::Translate(const DirectX::XMVECTOR& translation, eSpace relativeTo)
	{
		if (relativeTo == eSpace::World)
		{
			XMStoreFloat3(&m_LocalPosition, XMVectorAdd(GetLocalPosition(), translation));
			//XMStoreFloat3(&m_LookAt, XMVectorAdd(GetLookAt(), translation));
		}
		else
		{
			// 아, 헷갈려서 더 모르겠다.
			auto m = XMMatrixRotationQuaternion(GetLocalRotation());
			auto v = XMVector3Transform(translation, m);

			XMStoreFloat3(&m_LocalPosition, XMVectorAdd(GetLocalPosition(), v));
			//XMStoreFloat3(&m_LookAt, XMVectorAdd(GetLookAt(), v));
		}
	}

	void OldTransform::Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo)
	{
		float radianX = XMConvertToRadians(xAngle);
		float radianY = XMConvertToRadians(yAngle);
		float radianZ = XMConvertToRadians(zAngle);

		if (relativeTo == eSpace::World)
		{
			XMVECTOR v = XMVectorSet(radianX, radianY, radianZ, 1.0f);

			XMVECTOR n = XMVector3Normalize(v);
			XMVECTOR l = XMVector3Length(v);

			if (XMVector3NotEqual(n, XMVectorZero()) == true)
			{
				auto q = XMQuaternionRotationNormal(n, XMVectorGetX(l));
				SetLocalRotationQuaternion(XMQuaternionNormalize(XMQuaternionMultiply(GetLocalRotation(), q)));
			}
		}
		else
		{
			XMMATRIX m = XMMatrixRotationQuaternion(GetLocalRotation());
			XMVECTOR v = XMVector3Transform(XMVectorSet(radianX, radianY, radianZ, 1.0f), m);

			XMVECTOR n = XMVector3Normalize(v);
			XMVECTOR l = XMVector3Length(v);

			if (XMVector3NotEqual(n, XMVectorZero()) == true)
			{
				auto q = XMQuaternionRotationNormal(n, XMVectorGetX(l));
				SetLocalRotationQuaternion(XMQuaternionNormalize(XMQuaternionMultiply(GetLocalRotation(), q)));
			}
		}
	}

	void OldTransform::Rotate(const DirectX::XMFLOAT3& eulerAngle, eSpace relativeTo)
	{
		Rotate(eulerAngle.x, eulerAngle.y, eulerAngle.z, relativeTo);
	}

	void OldTransform::RotateQuaternion(const DirectX::XMFLOAT4& delta, eSpace relativeTo)
	{
		RotateQuaternion(XMLoadFloat4(&delta), relativeTo);
	}

	void OldTransform::RotateQuaternion(const DirectX::XMVECTOR& delta, eSpace relativeTo)
	{
		if (relativeTo == eSpace::World)
		{
			// 일단 이건 확실히 잘못된 듯 보인다.
			// spartan의 경우 rotationLocal * GetRotation().Inverse() * delta * GetRotation()이다. 
			//SetRotationQuaternion(XMQuaternionNormalize(XMQuaternionMultiply(GetRotation(), delta)));
		}
		else
		{
			SetLocalRotationQuaternion(XMQuaternionNormalize(XMQuaternionMultiply(GetLocalRotation(), delta)));
		}
	}

	DirectX::XMVECTOR OldTransform::GetLookAt()
	{
		// 스파르타은 그냥 이렇다.
		return XMVectorAdd(GetForward(), GetPosition());
	}

	// 현재 위치와 동일하면 안된다.
	// => 여기에서 너무 헤맸다. 일단 넘어가자.
	void OldTransform::LookAt(const OldTransform& target)
	{
		if (XMVector3Equal(GetPosition(), target.GetPosition()))
			return;

		XMStoreFloat3(&m_LookAt, target.GetPosition());
	}

	// 현재 위치와 동일하면 안된다.
	void OldTransform::LookAt(const DirectX::XMFLOAT3& worldPosition)
	{
		if (XMVector3Equal(GetPosition(), XMLoadFloat3(&worldPosition)))
			return;

		m_LookAt = worldPosition;
	}

	// 유니티에서는 전부 월드 스페이스다.
	DirectX::XMVECTOR OldTransform::GetForward() const
	{
		return XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), XMMatrixRotationQuaternion(GetLocalRotation()));
	}

	DirectX::XMVECTOR OldTransform::GetUp() const
	{
		return XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMMatrixRotationQuaternion(GetLocalRotation()));
	}

	DirectX::XMVECTOR OldTransform::GetRight() const
	{
		return XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMMatrixRotationQuaternion(GetLocalRotation()));
	}

	bool OldTransform::HasParent() const
	{
		return m_pParent;
	}

	void OldTransform::SetParent(OldTransform* pParent)
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

	OldTransform* OldTransform::GetRoot()
	{
		if (!m_pParent)
			return this;

		return m_pParent->GetRoot();
	}

	OldTransform* OldTransform::GetChild(uint32_t index) const
	{
		if (m_Children.empty() || static_cast<uint32_t>(m_Children.size()) < index)
			return nullptr;

		return m_Children[index];
	}

	bool OldTransform::IsChildOf(OldTransform* pParent) const
	{
		if (!pParent)
			return false;

		if (this->GetID() == pParent->GetID())
			return true;

		if (!m_pParent)
			return false;
		else
			return m_pParent->IsChildOf(pParent);
	}

	void OldTransform::DetachChildren()
	{
		for (auto* pChild : m_Children)
		{
			pChild->SetParent(nullptr);
		}
		m_Children.clear();
		m_Children.shrink_to_fit();
	}

	OldTransform* OldTransform::FindByName(const std::string& name)
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

	void OldTransform::updateTransform()
	{
		auto localMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&m_LocalScale)) *
			XMMatrixRotationQuaternion(XMLoadFloat4(&m_LocalRotation)) *
			XMMatrixTranslationFromVector(XMLoadFloat3(&m_LocalPosition));
		XMStoreFloat4x4(&m_LocalMatrix, localMatrix);

		if (m_pParent)
		{
			auto matrix = XMMatrixMultiply(localMatrix, m_pParent->GetMatrix());
			XMStoreFloat4x4(&m_Matrix, matrix);
		}
		else
		{
			m_Matrix = m_LocalMatrix;
		}

		for (auto pChild : m_Children)
		{
			pChild->updateTransform();
		}
	}
}
