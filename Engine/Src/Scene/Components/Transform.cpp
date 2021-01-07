#include "DivePch.h"
#include "Transform.h"
#include "Core/Context.h"
#include "Core/FileStream.h"
#include "Core/Log.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"


namespace Dive
{
	Transform::Transform(Context * context, GameObject * owner)
		: Component(context, owner),
		m_parent(nullptr)
	{
		m_type = eComponentType::Transform;

		m_localPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_localScale	= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		CORE_TRACE("Create {:s}'s Transform Component.", m_owner->GetName());
	}

	Transform::~Transform()
	{
		CORE_TRACE("Destroy {:s}'s Transform Component.", m_owner->GetName());
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
	}

	void Transform::updateTransform()
	{
		// 변경된 hierarchy에 맞춰 변환 행렬 계산
		DirectX::XMMATRIX localPositionMatrix = DirectX::XMMatrixTranslationFromVector(GetLocalPosition());

		auto localMatrix = localPositionMatrix;
		DirectX::XMStoreFloat4x4(&m_localMatrix, localMatrix);

		if (m_parent)
			DirectX::XMStoreFloat4x4(&m_matrix, (localMatrix * m_parent->GetMatrix()));
		else
			m_matrix = m_localMatrix;

		// 자식들도 전부 다시 계산
		for (const auto& child : m_children)
		{
			child->updateTransform();
		}
	}
}