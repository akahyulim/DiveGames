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

		static constexpr eComponentType GetComponentType() { return eComponentType::Transform; }

		// transform
		DirectX::XMVECTOR GetPositionVector() const;
		DirectX::XMFLOAT3 GetPosition() const;
		void SetPositionVector(const DirectX::XMVECTOR& position);
		void SetPosition(const DirectX::XMFLOAT3& position);
		DirectX::XMVECTOR GetLocalPositionVector() const { return DirectX::XMLoadFloat3(&m_localPosition); }
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_localPosition; }
		void SetLocalPositionVector(const DirectX::XMVECTOR& position) { DirectX::XMStoreFloat3(&m_localPosition, position); }
		void SetLocalPosition(const DirectX::XMFLOAT3& position) { m_localPosition = position; }

		DirectX::XMVECTOR GetRotationVector() const;
		DirectX::XMFLOAT4 GetRotation() const;
		DirectX::XMFLOAT3 GetRotationRadians() const;
		DirectX::XMFLOAT3 GetRotationDegrees() const;
		void SetRotationVector(const DirectX::XMVECTOR& quaternion);
		void SetRotation(const DirectX::XMFLOAT4& quaternion);
		void SetRotationByRadians(const DirectX::XMFLOAT3& radians);
		void SetRotationByDegrees(const DirectX::XMFLOAT3& degrees);
		DirectX::XMVECTOR GetLocalRotationVector() const { return DirectX::XMLoadFloat4(&m_localRotation); }
		DirectX::XMFLOAT4 GetLocalRotation() const { return m_localRotation; }
		DirectX::XMFLOAT3 GetLocalRotationRadians() const;
		DirectX::XMFLOAT3 GetLocalRotationDegrees() const;
		void SetLocalRotationVector(const DirectX::XMVECTOR& quaternion);
		void SetLocalRotation(const DirectX::XMFLOAT4& quaternion);
		void SetLocalRotationByRadians(const DirectX::XMFLOAT3& radians);
		void SetLocalRotationByDegrees(const DirectX::XMFLOAT3& degrees);

		DirectX::XMVECTOR GetScaleVector() const;
		DirectX::XMFLOAT3 GetScale() const;
		void SetScaleVector(const DirectX::XMVECTOR& scale);
		void SetScale(const DirectX::XMFLOAT3& scale);
		DirectX::XMVECTOR GetLocalScaleVector() const { return DirectX::XMLoadFloat3(&m_localScale); }
		DirectX::XMFLOAT3 GetLocalScale() const { return m_localScale; }
		void SetLocalScaleVector(const DirectX::XMVECTOR& scale) { DirectX::XMStoreFloat3(&m_localScale, scale); }
		void SetLocalScale(const DirectX::XMFLOAT3& scale) { m_localScale = scale; }

		DirectX::XMMATRIX GetTransformMatrix() const;
		DirectX::XMFLOAT4X4 GetTransform() const;
		DirectX::XMMATRIX GetLocalTransformMatrix() const;
		DirectX::XMFLOAT4X4 GetLocalTransform() const;
		
		void Translate(const DirectX::XMFLOAT3& move, eSpace space = eSpace::Self);
		void Rotate(const DirectX::XMFLOAT4& quaternion, eSpace space = eSpace::Self);
		void RotateByRadians(const DirectX::XMFLOAT3& radians, eSpace space = eSpace::Self);
		void RotateByDegrees(const DirectX::XMFLOAT3& degrees, eSpace space = eSpace::Self);

		DirectX::XMVECTOR GetForward() const { return DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); }
		DirectX::XMVECTOR GetLocalForward() const;
		DirectX::XMVECTOR GetBackward() const{ return DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); }
		DirectX::XMVECTOR GetLocalBackward() const;
		DirectX::XMVECTOR GetRight() const { return DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); }
		DirectX::XMVECTOR GetLocalRight() const;
		DirectX::XMVECTOR GetLeft() const { return DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f); }
		DirectX::XMVECTOR GetLocalLeft() const;
		DirectX::XMVECTOR GetUp() const { return DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); }
		DirectX::XMVECTOR GetLocalUp() const;
		DirectX::XMVECTOR GetDown() const { return DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); }
		DirectX::XMVECTOR GetLocalDown() const;

		// hierarhy
		bool HasParent() const { return m_parent != nullptr; }
		Transform* GetParent() const { return m_parent; }
		void SetParent(Transform* parent);

		bool HasChildren() const { return !m_children.empty(); }
		std::vector<Transform*>& GetChildren() { return m_children; }

		bool IsChildOf(Transform* parent);

		Transform* GetChild(size_t index);
		Transform* Find(const std::string& name);

		size_t GetChildCount();

		void DetachChildren();
		void RemoveChild(Transform* child);

		size_t GetSiblingIndex();
		void SetSiblingIndex(size_t index);

	private:
		DirectX::XMFLOAT3 m_localPosition{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 m_localRotation{ 0.0f, 0.0f, 0.0f, 1.0f };	// quaternion
		DirectX::XMFLOAT3 m_localScale{ 1.0f, 1.0f, 1.0f };

		Transform* m_parent = nullptr;
		std::vector<Transform*> m_children;
	};
}
