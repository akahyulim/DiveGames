#include "DivePch.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "IO/Log.h"

using namespace DirectX;

namespace Dive
{
	Transform::Transform()
		: Component(nullptr),
		m_pParent(nullptr)
	{
		m_LocalPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LocalScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

		m_LookAt = XMFLOAT3(0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_Matrix, XMMatrixIdentity());
	}

	Transform::Transform(GameObject* pGameObject)
		: Component(pGameObject),
		m_pParent(nullptr)
	{
		m_LocalPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LocalScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

		m_LookAt = XMFLOAT3(0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_Matrix, XMMatrixIdentity());
	}

	Transform::~Transform()
	{
		DV_CORE_TRACE("Destroy Transform: {:s}", GetName());
	}

	bool Transform::LoadFromFile(const std::string& filePath)
	{
		return true;
	}

	bool Transform::SaveToFile(const std::string& filePath)
	{
		DV_CORE_INFO("Save Transform: {:d}", GetID());

		return true;
	}

	bool Transform::SaveToYAML(YAML::Emitter& out)
	{
		//out << YAML::BeginMap;
		out << YAML::Key << "Transform" << YAML::Value << GetID();
		//out << YAML::EndMap;

		return true;
	}

	void Transform::Update(float delta)
	{
		updateTransform();
	}

	void Transform::Clear()
	{
		SetPosition(0.0f, 0.0f, 0.0f);
		SetRotation(0.0f, 0.0f, 0.0f);
		SetScale(1.0f, 1.0f, 1.0f);
	}

	DirectX::XMVECTOR Transform::GetPosition() const
	{
		return XMVectorSet(m_Matrix._41, m_Matrix._42, m_Matrix._43, 1.0f);
	}

	DirectX::XMVECTOR Transform::GetLocalPosition() const
	{
		return XMLoadFloat3(&m_LocalPosition);
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPosition(XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& pos)
	{
		SetPosition(XMLoadFloat3(&pos));
	}

	void Transform::SetPosition(const DirectX::XMVECTOR& pos)
	{
		if (XMVector3Equal(GetPosition(), pos))
			return;

		SetLocalPosition(
			HasParent() ? XMVector3Transform(pos, XMMatrixInverse(nullptr, m_pParent->GetMatrix())) : pos);
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPosition(XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& pos)
	{
		SetLocalPosition(XMLoadFloat3(&pos));
	}

	void Transform::SetLocalPosition(const DirectX::XMVECTOR& pos)
	{
		if (XMVector3Equal(GetLocalPosition(), pos))
			return;

		XMStoreFloat3(&m_LocalPosition, pos);

		updateTransform();
	}

	DirectX::XMVECTOR Transform::GetRotation() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&m_Matrix));

		return rot;
	}

	DirectX::XMVECTOR Transform::GetLocalRotation() const
	{
		return XMLoadFloat4(&m_LocalRotation);
	}

	void Transform::SetRotation(float xAngle, float yAngle, float zAngle)
	{
		float radianX = XMConvertToRadians(xAngle);
		float radianY = XMConvertToRadians(yAngle);
		float radianZ = XMConvertToRadians(zAngle);

		SetRotationQuaternion(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetRotation(const DirectX::XMFLOAT3& angle)
	{
		float radianX = XMConvertToRadians(angle.x);
		float radianY = XMConvertToRadians(angle.y);
		float radianZ = XMConvertToRadians(angle.z);

		SetRotationQuaternion(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetRotationQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotationQuaternion(XMLoadFloat4(&quaternion));
	}

	void Transform::SetRotationQuaternion(const DirectX::XMVECTOR& quaternion)
	{
		if (XMVector4Equal(GetRotation(), quaternion))
			return;

		SetLocalRotationQuaternion(
			HasParent() ? XMQuaternionMultiply(quaternion, XMQuaternionInverse(m_pParent->GetRotation())) : quaternion);
	}

	void Transform::SetLocalRotation(float xAngle, float yAngle, float zAngle)
	{
		float radianX = XMConvertToRadians(xAngle);
		float radianY = XMConvertToRadians(yAngle);
		float radianZ = XMConvertToRadians(zAngle);

		SetLocalRotationQuaternion(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT3& angle)
	{
		float radianX = XMConvertToRadians(angle.x);
		float radianY = XMConvertToRadians(angle.y);
		float radianZ = XMConvertToRadians(angle.z);

		SetLocalRotationQuaternion(XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	void Transform::SetLocalRotationQuaternion(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotationQuaternion(XMLoadFloat4(&quaternion));
	}

	void Transform::SetLocalRotationQuaternion(const DirectX::XMVECTOR& quaternion)
	{
		if (XMVector4Equal(GetLocalRotation(), quaternion))
			return;

		XMStoreFloat4(&m_LocalRotation, quaternion);

		updateTransform();
	}

	DirectX::XMVECTOR Transform::GetScale() const
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, GetMatrix());

		return scale;
	}

	DirectX::XMVECTOR Transform::GetLocalScale() const
	{
		return XMLoadFloat3(&m_LocalScale);
	}

	void Transform::SetScale(float x, float y, float z)
	{
		if ((x == 0.0f) || (y == 0.0f) || (z == 0.0f))
			return;

		SetScale(XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		if ((scale.x == 0.0f) || (scale.y == 0.0f) || (scale.z == 0.0f))
			return;

		SetScale(XMLoadFloat3(&scale));
	}

	void Transform::SetScale(const DirectX::XMVECTOR& scale)
	{
		if (XMVector3Equal(GetScale(), scale))
			return;

		SetLocalScale(HasParent() ? XMVectorDivide(scale, m_pParent->GetScale()) : scale);
	}

	void Transform::SetLocalScale(float x, float y, float z)
	{
		if ((x == 0.0f) || (y == 0.0f) || (z == 0.0f))
			return;

		SetLocalScale(XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		if ((scale.x == 0.0f) || (scale.y == 0.0f) || (scale.z == 0.0f))
			return;

		SetLocalScale(XMLoadFloat3(&scale));
	}

	void Transform::SetLocalScale(const DirectX::XMVECTOR& scale)
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

	XMMATRIX Transform::GetMatrix() const
	{
		return XMLoadFloat4x4(&m_Matrix);
	}

	void Transform::SetMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&matrix));

		SetPosition(pos);
		SetRotationQuaternion(rot);
		SetScale(scale);
	}

	XMMATRIX Transform::GetLocalMatrix() const
	{
		return XMLoadFloat4x4(&m_LocalMatrix);
	}

	void Transform::SetLocalMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		XMVECTOR pos, rot, scale;
		XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&matrix));

		SetLocalPosition(pos);
		SetLocalRotationQuaternion(rot);
		SetLocalScale(scale);
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		Translate(XMVectorSet(x, y, z, 0.0f), relativeTo);
	}

	void Transform::Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo)
	{
		Translate(XMLoadFloat3(&translation), relativeTo);
	}

	void Transform::Translate(const DirectX::XMVECTOR& translation, eSpace relativeTo)
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

	void Transform::Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo)
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

	void Transform::Rotate(const DirectX::XMFLOAT3& eulerAngle, eSpace relativeTo)
	{
		Rotate(eulerAngle.x, eulerAngle.y, eulerAngle.z, relativeTo);
	}

	void Transform::RotateQuaternion(const DirectX::XMFLOAT4& delta, eSpace relativeTo)
	{
		RotateQuaternion(XMLoadFloat4(&delta), relativeTo);
	}

	void Transform::RotateQuaternion(const DirectX::XMVECTOR& delta, eSpace relativeTo)
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

	DirectX::XMVECTOR Transform::GetLookAt()
	{
		// 스파르타은 그냥 이렇다.
		return XMVectorAdd(GetForward(), GetPosition());
	}

	// 현재 위치와 동일하면 안된다.
	// => 여기에서 너무 헤맸다. 일단 넘어가자.
	void Transform::LookAt(const Transform& target)
	{
		if (XMVector3Equal(GetPosition(), target.GetPosition()))
			return;

		XMStoreFloat3(&m_LookAt, target.GetPosition());
	}

	// 현재 위치와 동일하면 안된다.
	void Transform::LookAt(const DirectX::XMFLOAT3& worldPosition)
	{
		if (XMVector3Equal(GetPosition(), XMLoadFloat3(&worldPosition)))
			return;

		m_LookAt = worldPosition;
	}

	DirectX::XMVECTOR Transform::GetForward() const
	{
		return XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), XMMatrixRotationQuaternion(GetLocalRotation()));
	}

	DirectX::XMVECTOR Transform::GetUp() const
	{
		return XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMMatrixRotationQuaternion(GetLocalRotation()));
	}

	DirectX::XMVECTOR Transform::GetRight() const
	{
		return XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMMatrixRotationQuaternion(GetLocalRotation()));
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
		if (!pParent)
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

	void Transform::updateTransform()
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