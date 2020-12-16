#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class GameObject;

	// 일단 계층 구조와 위치 설정만 구현
	class Transform : public Component
	{
		DIVE_OBJECT(Transform, Component);

	public:
		explicit Transform(Context* context, GameObject* owner);
		~Transform();

		// serialization

		// const &로 리턴???
		//= position
		DirectX::XMVECTOR GetPosition();
		DirectX::XMVECTOR GetLocalPosition();
		void SetPosition(const DirectX::XMVECTOR& position);
		void SetLocalPosition(const DirectX::XMVECTOR& position);
		void SetPosition(float x, float y, float z)			{ SetPosition(DirectX::XMVectorSet(x, y, z, 1.0f)); }
		void SetLocalPosition(float x, float y, float z)	{ SetLocalPosition(DirectX::XMVectorSet(x, y, z, 1.0f)); }

		//= etc
		DirectX::XMMATRIX GetMatrix()		const { return DirectX::XMLoadFloat4x4(&m_matrix); }
		DirectX::XMMATRIX GetLocalMatrix()	const { return DirectX::XMLoadFloat4x4(&m_localMatrix); }

		//= hierarchy
		bool IsRoot()									const { return !HasParent(); }
		bool HasParent()								const { return m_parent; }
		void SetParent(Transform* parent);
		void BecomeOrphan();
		bool IsDescendant(const Transform* transform) const;
		bool HasChildren()								const { !m_children.empty() ? true : false; }
		size_t GetChildrenCount()						const { return m_children.size(); }
		void AddChild(Transform* child);
		Transform* GetRoot();
		Transform* GetParent() const { return m_parent; }
		Transform* GetChildByName(const std::string& name);
		Transform* GetChildByIndex(unsigned int index);
		const std::vector<Transform*>& GetChildren()	const { return m_children; }


	private:
		void updateTransform();

	private:
		DirectX::XMFLOAT3 m_localPosition;

		DirectX::XMFLOAT4X4 m_matrix;
		DirectX::XMFLOAT4X4 m_localMatrix;

		Transform* m_parent;
		std::vector<Transform*> m_children;
	};
}