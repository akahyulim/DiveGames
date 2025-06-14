#pragma once
#include "Component.h"

namespace Dive
{
	class Transform : public Component
	{
	public:
		Transform(GameObject* gameObject);
		~Transform() override;

		static constexpr eComponentType GetType() { return eComponentType::Transform; }

		// transform
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_Position; }
		void SetLocalPosition(const DirectX::XMFLOAT3& position) { m_Position = position; }

		DirectX::XMFLOAT4 GetLocalRotation() const { return m_Rotation; }
		void SetLocalRotation(const DirectX::XMFLOAT4& rotation) { m_Rotation = rotation; }

		DirectX::XMFLOAT3 GetLocalScale() const { return m_Scale; }
		void SetLocalScale(const DirectX::XMFLOAT3& scale) { m_Scale = scale; }

		// hierarhy
		bool HasParent() const { return m_Parent != nullptr; }
		Transform* GetParent() const { return m_Parent; }
		void SetParent(Transform* parent);

		bool HasChildren() const { return !m_Children.empty(); }
		std::vector<Transform*> GetChildren() const { return m_Children; }

		bool IsChildOf(Transform* parent);

		Transform* GetChild(size_t index);
		Transform* Find(const std::string& name);

		size_t GetChildCount();

		void DetachChildren();
		void RemoveChild(Transform* child);

	private:
	private:
		DirectX::XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 m_Rotation{ 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT3 m_Scale{ 1.0f, 1.0f, 1.0f };

		Transform* m_Parent = nullptr;
		std::vector<Transform*> m_Children;
	};
}