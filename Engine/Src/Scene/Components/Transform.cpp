#include "DivePch.h"
#include "Transform.h"
#include "Core/Context.h"
#include "Core/FileStream.h"
#include "Core/Log.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"
#include "Geometry/Math.h"


namespace Dive
{
	Transform::Transform(Context * context, GameObject * owner)
		: Component(context, owner),
		m_parent(nullptr)
	{
		m_componentType = eComponentType::Transform;

		m_localPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_localRotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_localScale	= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		DirectX::XMStoreFloat4x4(&m_localMatrix, DirectX::XMMatrixIdentity());
		m_matrix = m_localMatrix;

		// start 등에서?
		updateTransform();
	}

	Transform::~Transform()
	{
	}

	void Transform::Serialize(FileStream & stream)
	{
		stream.Write(m_localPosition);
		// local rotation
		stream.Write(m_localScale);
		// look at
		stream.Write(m_parent ? m_parent->GetID() : 0);
	}

	void Transform::Deserialize(FileStream & stream)
	{
		stream.Read(&m_localPosition);
		// local rotation
		stream.Read(&m_localScale);
		// look at
		unsigned int parentID = 0;
		stream.Read(&parentID);

		if (parentID != 0)
		{
			if (const auto& parent = GetSubsystem<Scene>()->GetGameObjectByID(parentID))
			{
				parent->GetComponent<Transform>()->AddChild(this);
			}
		}

		updateTransform();
	}

	void Transform::Copy(Component * other)
	{
		if (!other)
			return;

		auto org = dynamic_cast<Transform*>(other);
		if (!org)
			return;

		m_localPosition = org->m_localPosition;
		// local rotation
		m_localScale = org->m_localScale;
		// look at
	}

	DirectX::XMVECTOR Transform::GetPosition()
	{
		return DirectX::XMVectorSet(m_matrix._41, m_matrix._42, m_matrix._43, 1.0f);
	}

	DirectX::XMVECTOR Transform::GetLocalPosition()
	{
		return DirectX::XMLoadFloat3(&m_localPosition);
	}

	void Transform::SetPosition(const DirectX::XMVECTOR & position)
	{
		if (DirectX::XMVector3Equal(position, GetPosition()))
			return;

		SetLocalPosition(IsRoot() ? 
			position : DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, m_parent->GetMatrix())));
	}

	void Transform::SetLocalPosition(const DirectX::XMVECTOR & position)
	{
		if (DirectX::XMVector3Equal(position, GetLocalPosition()))
			return;

		DirectX::XMStoreFloat3(&m_localPosition, position);
		updateTransform();

		DirectX::XMFLOAT3 world;
		DirectX::XMStoreFloat3(&world, GetPosition());
		CORE_TRACE("Position - {0:s}'s world: {1:f}, {2:f}, {3:f}", GetName().c_str(), world.x, world.y, world.z);
		CORE_TRACE("Position - {0:s}'s local: {1:f}, {2:f}, {3:f}", GetName().c_str(), m_localPosition.x, m_localPosition.y, m_localPosition.z);
	}

	DirectX::XMVECTOR Transform::GetRotation()
	{
		return Math::GetRotationQuaternion(GetMatrix(), GetScale());
	}

	DirectX::XMVECTOR Transform::GetRotationDegrees()
	{
		return Math::QuaternionConvertDegrees(GetRotation());
	}

	DirectX::XMVECTOR Transform::GetRotationRadians()
	{
		// 다시 변환
		return DirectX::XMVECTOR();
	}

	DirectX::XMVECTOR Transform::GetLocalRotation()
	{
		return DirectX::XMLoadFloat4(&m_localRotation);
	}

	DirectX::XMVECTOR Transform::GetLocalRotationDegrees()
	{
		return Math::QuaternionConvertDegrees(GetLocalRotation());
	}

	DirectX::XMVECTOR Transform::GetLocalRotationRadians()
	{
		return DirectX::XMVECTOR();
	}

	void Transform::SetRotation(const DirectX::XMVECTOR & quaternion)
	{
		if (DirectX::XMVector4Equal(quaternion, GetRotation()))
			return;

		SetLocalRotation(IsRoot() ?
			quaternion : DirectX::XMQuaternionMultiply(quaternion, DirectX::XMQuaternionInverse(m_parent->GetRotation())));
	}

	void Transform::SetLocalRotation(const DirectX::XMVECTOR & quaternion)
	{
		if (DirectX::XMVector4Equal(quaternion, GetLocalRotation()))
			return;

		DirectX::XMStoreFloat4(&m_localRotation, quaternion);
		updateTransform();
	}

	void Transform::SetRotationDegrees(const DirectX::XMVECTOR & degrees)
	{
		DirectX::XMFLOAT3 angles;
		DirectX::XMStoreFloat3(&angles, degrees);

		float rx = DirectX::XMConvertToRadians(angles.x);
		float ry = DirectX::XMConvertToRadians(angles.y);
		float rz = DirectX::XMConvertToRadians(angles.z);

		SetRotationRadians(DirectX::XMVectorSet(rx, ry, rz, 1.0f));
	}

	void Transform::SetLocalRotationDegrees(const DirectX::XMVECTOR & degrees)
	{
		DirectX::XMFLOAT3 angles;
		DirectX::XMStoreFloat3(&angles, degrees);

		float rx = DirectX::XMConvertToRadians(angles.x);
		float ry = DirectX::XMConvertToRadians(angles.y);
		float rz = DirectX::XMConvertToRadians(angles.z);

		SetLocalRotationRadians(DirectX::XMVectorSet(rx, ry, rz, 1.0f));
	}

	void Transform::SetRotationRadians(const DirectX::XMVECTOR & radians)
	{
		DirectX::XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&rot, radians);

		auto quaternion = DirectX::XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);

		SetRotation(quaternion);
	}

	void Transform::SetLocalRotationRadians(const DirectX::XMVECTOR & radians)
	{
		DirectX::XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&rot, radians);

		auto quaternion = DirectX::XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);

		SetLocalRotation(quaternion);
	}

	DirectX::XMVECTOR Transform::GetScale()
	{
		return Math::GetScale(GetMatrix());
	}

	DirectX::XMVECTOR Transform::GetLocalScale()
	{
		return DirectX::XMLoadFloat3(&m_localScale);
	}

	void Transform::SetScale(const DirectX::XMVECTOR & scale)
	{
		if (DirectX::XMVector3Equal(scale, GetScale()))
			return;

		if (DirectX::XMVectorGetX(scale) <= 0.00f || DirectX::XMVectorGetY(scale) <= 0.00f || DirectX::XMVectorGetZ(scale) <= 0.00f)
		{
			CORE_ERROR("");
			return;
		}

		SetLocalScale(HasParent() ? DirectX::XMVectorDivide(scale, m_parent->GetScale()) : scale);
	}

	void Transform::SetLocalScale(const DirectX::XMVECTOR & scale)
	{
		if (DirectX::XMVector3Equal(scale, GetLocalScale()))
			return;

		if (DirectX::XMVectorGetX(scale) <= 0.00f || DirectX::XMVectorGetY(scale) <= 0.00f || DirectX::XMVectorGetZ(scale) <= 0.00f)
		{
			CORE_ERROR("");
			return;
		}

		DirectX::XMStoreFloat3(&m_localScale, scale);
		updateTransform();

		DirectX::XMFLOAT3 world;
		DirectX::XMStoreFloat3(&world, GetScale());
		CORE_TRACE("Scale - {0:s}'s world: {1:f}, {2:f}, {3:f}", GetName().c_str(), world.x, world.y, world.z);
		CORE_TRACE("Scale - {0:s}'s local: {1:f}, {2:f}, {3:f}", GetName().c_str(), m_localScale.x, m_localScale.y, m_localScale.z);
	}

	void Transform::Translate(const DirectX::XMVECTOR & delta)
	{
		if (HasParent())
		{
			auto inverse = DirectX::XMMatrixInverse(nullptr, m_parent->GetMatrix());
			SetLocalPosition(DirectX::XMVectorAdd(GetLocalPosition(), DirectX::XMVector3Transform(delta, inverse)));
		}
		else
		{
			SetLocalPosition(DirectX::XMVectorAdd(GetLocalPosition(), delta));
		}
	}

	void Transform::Rotate(const DirectX::XMVECTOR & delta)
	{
		if (HasParent())
		{
			// 흐음.... 부모 회전을 사용하지 않는다.
			// 회전 공식과 비슷하다.
		}
		else
		{
			SetLocalRotation(DirectX::XMQuaternionNormalize(DirectX::XMQuaternionMultiply(GetLocalRotation(), delta)));
		}
	}

	DirectX::XMVECTOR Transform::GetUp()
	{
		auto mat = DirectX::XMMatrixRotationQuaternion(GetLocalRotation());
		return DirectX::XMVector4Transform(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), mat);
	}

	DirectX::XMVECTOR Transform::GetDown()
	{
		auto mat = DirectX::XMMatrixRotationQuaternion(GetLocalRotation());
		return DirectX::XMVector4Transform(DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), mat);
	}

	DirectX::XMVECTOR Transform::GetLeft()
	{
		auto mat = DirectX::XMMatrixRotationQuaternion(GetLocalRotation());
		return DirectX::XMVector4Transform(DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f), mat);
	}

	DirectX::XMVECTOR Transform::GetRight()
	{
		auto mat = DirectX::XMMatrixRotationQuaternion(GetLocalRotation());
		return DirectX::XMVector4Transform(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), mat);
	}

	DirectX::XMVECTOR Transform::GetBackward()
	{
		auto mat = DirectX::XMMatrixRotationQuaternion(GetLocalRotation());
		return DirectX::XMVector4Transform(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), mat);
	}

	DirectX::XMVECTOR Transform::GetForward()
	{
		auto mat = DirectX::XMMatrixRotationQuaternion(GetLocalRotation());
		return DirectX::XMVector4Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), mat);
	}

	void Transform::updateTransform()
	{
		auto localMatrix = Math::CalcurateTransformMatrix(GetLocalPosition(), GetLocalRotation(), GetLocalScale());
		DirectX::XMStoreFloat4x4(&m_localMatrix, localMatrix);

		if (IsRoot())
		{
			m_matrix = m_localMatrix;
		}
		else
		{
			DirectX::XMStoreFloat4x4(&m_matrix, DirectX::XMMatrixMultiply(localMatrix, m_parent->GetMatrix()));
		}

		for (const auto& child : m_children)
		{
			child->updateTransform();
		}
	}
}