#include "DivePch.h"
#include "Transform.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Scene/GameObject.h"


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

	void Transform::SetParent(Transform * parent)
	{
		// 빈 객체를 전달받으면 독립
		if (!m_parent)
		{
			BecomeOrphan();
			return;
		}

		// 자신을 부모를 삼으려는 경우
		if (GetID() == parent->GetID())
			return;

		// 동일한 부모를 전달받은 경우
		if (m_parent->GetID() == parent->GetID())
			return;

		// 자식 혹은 자손을 전달받은 경우
		if (IsDescendant(parent))
			return;

		// 기존 부모에게서 제거
		if (m_parent)
		{
			auto it = m_parent->GetChildren().begin();
			for (it; it != m_parent->GetChildren().end();)
			{
				if ((*it)->GetID() == GetID())
				{
					it = m_parent->m_children.erase(it);
				}
				else
					it++;
			}
		}

		// 새로운 부모에게 추가
		parent->AddChild(this);
		m_parent = parent;

		updateTransform();
	}
	
	void Transform::BecomeOrphan()
	{
		if (!m_parent)
			return;

		// 기존 부모에게서 제거
		auto it = m_parent->GetChildren().begin();
		for (it; it != m_parent->GetChildren().end();)
		{
			if ((*it)->GetID() == GetID())
			{
				it = m_parent->m_children.erase(it);
			}
			else
				it++;
		}

		m_parent = nullptr;

		updateTransform();
	}

	bool Transform::IsDescendant(const Transform * transform) const
	{
		if (!transform || transform->IsRoot())
			return false;

		for (const auto& child : m_children)
		{
			if (child->GetID() == transform->GetID())
				return true;

			if (child->IsDescendant(transform))
				return true;
		}

		return false;
	}
	
	void Transform::AddChild(Transform * child)
	{
		if (!child || GetID() == child->GetID())
			return;

		child->SetParent(this);
	}
	
	Transform * Transform::GetRoot()
	{
		if (!m_parent)
			return this;

		m_parent->GetRoot();
	}
	
	Transform * Transform::GetChildByName(const std::string & name)
	{
		if (m_children.empty() || name.empty())
			return nullptr;

		for (const auto& child : m_children)
		{
			if (child->GetName() == name)
				return child;
		}

		return nullptr;
	}
	
	Transform * Transform::GetChildByIndex(unsigned int index)
	{
		if (m_children.empty() || index > m_children.size())
			return nullptr;

		return m_children[index];
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