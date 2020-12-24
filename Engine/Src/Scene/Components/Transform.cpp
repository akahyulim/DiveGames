#include "DivePch.h"
#include "Transform.h"
#include "Core/Context.h"
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

		CORE_TRACE("Create Transform.");
	}

	Transform::~Transform()
	{
		CORE_TRACE("Destroy Transform.");
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