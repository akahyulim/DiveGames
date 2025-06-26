#pragma once
#include "Component.h"

namespace Dive
{
	enum class eSpace
	{
		World,
		Self
	};

	// https://docs.unity3d.com/ScriptReference/Transform.html
	class Transform : public Component
	{
	public:
		Transform(GameObject* gameObject);
		~Transform() override;

		static constexpr eComponentType GetType() { return eComponentType::Transform; }

		// transform
		DirectX::XMFLOAT3 GetPosition() const;
		void SetPosition(const DirectX::XMFLOAT3& position);
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		void SetLocalPosition(const DirectX::XMFLOAT3& position) { m_LocalPosition = position; }

		DirectX::XMFLOAT4 GetRotationQuaternion() const;
		DirectX::XMFLOAT3 GetRotationRadians() const;
		DirectX::XMFLOAT3 GetRotationDegrees() const;
		void SetRotationByQuaternion(const DirectX::XMFLOAT4& quaternion);
		void SetRotationByRadians(const DirectX::XMFLOAT3& radians);
		void SetRotationByDegrees(const DirectX::XMFLOAT3& degrees);
		DirectX::XMFLOAT4 GetLocalRotationQuaternion() const { return m_LocalRotation; }
		DirectX::XMFLOAT3 GetLocalRotationRadians() const;
		DirectX::XMFLOAT3 GetLocalRotationDegrees() const;
		void SetLocalRotationQuaternion(const DirectX::XMFLOAT4& quaternion);
		void SetLocalRotationByRadians(const DirectX::XMFLOAT3& radians);
		void SetLocalRotationByDegrees(const DirectX::XMFLOAT3& degrees);

		DirectX::XMFLOAT3 GetScale() const;
		void SetScale(const DirectX::XMFLOAT3& scale);
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		void SetLocalScale(const DirectX::XMFLOAT3& scale) { m_LocalScale = scale; }

		// 유니티에선 localToWorldMatrix, worldToLocalMatrix로 칭하는 듯 하다.
		DirectX::XMFLOAT4X4 GetTransform() const;
		DirectX::XMFLOAT4X4 GetLocalTransform() const;

		void Translate(const DirectX::XMFLOAT3& move, eSpace space = eSpace::Self);
		void RotateByQuaternion(const DirectX::XMFLOAT4& quaternion, eSpace space = eSpace::Self);
		void RotateByRadians(const DirectX::XMFLOAT3& radians, eSpace space = eSpace::Self);
		void RotateByDegrees(const DirectX::XMFLOAT3& degrees, eSpace space = eSpace::Self);

		// hierarhy
		bool HasParent() const { return m_Parent != nullptr; }
		Transform* GetParent() const { return m_Parent; }
		void SetParent(Transform* parent);

		bool HasChildren() const { return !m_Children.empty(); }
		std::vector<Transform*>& GetChildren() { return m_Children; }

		bool IsChildOf(Transform* parent);

		Transform* GetChild(size_t index);
		Transform* Find(const std::string& name);

		size_t GetChildCount();

		void DetachChildren();
		void RemoveChild(Transform* child);

		size_t GetSiblingIndex();
		void SetSiblingIndex(size_t index);

	private:
		DirectX::XMFLOAT3 m_LocalPosition{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 m_LocalRotation{ 0.0f, 0.0f, 0.0f, 1.0f };	// quaternion
		DirectX::XMFLOAT3 m_LocalScale{ 1.0f, 1.0f, 1.0f };

		Transform* m_Parent = nullptr;
		std::vector<Transform*> m_Children;
	};
}