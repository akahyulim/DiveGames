#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	enum class eSpace
	{
		World,
		Self
	};

	class Transform : public Component
	{
	public:
		Transform(GameObject* pGameObject);
		~Transform() = default;

		void Clear();

		// position
		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMVECTOR GetPositionVector() const;
		void SetPosition(const DirectX::XMFLOAT3& trans);
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalTranslation; }
		void SetLocalPosition(const DirectX::XMFLOAT3& trans) { m_LocalTranslation = trans; }

		// rotation
		DirectX::XMFLOAT4 GetRotation() const;
		DirectX::XMVECTOR GetRotationVector() const;
		DirectX::XMFLOAT3 GetRotationDegrees() const;
		void SetRotation(const DirectX::XMFLOAT4& rotQuat);
		void SetRotation(const DirectX::XMFLOAT3& degrees);
		DirectX::XMFLOAT4 GetLocalRotation() const { return m_LocalRotation;}
		DirectX::XMFLOAT3 GetLocalRotationDegrees() const;
		void SetLocalRotation(const DirectX::XMFLOAT4& rotQuat) { m_LocalRotation = rotQuat; }
		void SetLocalRotation(const DirectX::XMFLOAT3& degrees);

		// Scale
		DirectX::XMFLOAT3 GetScale() const;
		DirectX::XMVECTOR GetScaleVector() const;
		void SetScale(const DirectX::XMFLOAT3& scale);
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		void SetLocalScale(const DirectX::XMFLOAT3& scale) { m_LocalScale = scale; }

		// matrix
		DirectX::XMMATRIX GetMatrix();
		DirectX::XMMATRIX GetLocalMatrix() const;

		// Translate
		void Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo = eSpace::Self);
		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);

		// Rotate
		void Rotate(const DirectX::XMFLOAT3& degrees, eSpace relativeTo = eSpace::Self);
		void Rotate(const DirectX::XMVECTOR& rotQuat, eSpace relativeTo = eSpace::Self);
		void Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo = eSpace::Self);
		void Rotate(DirectX::XMFLOAT3 axis, float angle, eSpace relativeTo = eSpace::Self);

		// Direction
		DirectX::XMFLOAT3 GetForward();
		DirectX::XMFLOAT3 GetUp();
		DirectX::XMFLOAT3 GetRight();

		// LookAt
		void LookAt(Transform target, DirectX::XMFLOAT3 worldUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

		// Hierarchy
		bool HasParent() const { return (m_pParent != nullptr); }
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);
		Transform* GetRoot();
		bool HasChildren() const;
		std::vector<Transform*> GetChildren() const;
		unsigned int GetChildCount();
		Transform* GetChild(int index);
		bool IsChildOf(const Transform* pParent);
		void DetachChildren();
		
	private:
		DirectX::XMFLOAT3 m_LocalTranslation;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Matrix;

		Transform* m_pParent = nullptr;
		std::vector<Transform*> m_Children;
	};
}