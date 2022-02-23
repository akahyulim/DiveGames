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
		DirectX::XMFLOAT3 GetPosition(); 
		void SetPosition(DirectX::XMFLOAT3 pos);

		// rotation
		DirectX::XMFLOAT3 GetRotation();
		void SetRotation(DirectX::XMFLOAT3 rot);

		// matrix
		DirectX::XMFLOAT4X4 GetLocalMatrix();
		DirectX::XMFLOAT4X4 GetMatrix();

		// Translate
		void Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo = eSpace::Self);
		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);

		// Rotate
		void Rotate(const DirectX::XMFLOAT3& eulerAngles, eSpace relativeTo = eSpace::Self);
		void Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo = eSpace::Self);
		void Rotate(const DirectX::XMFLOAT3& axis, float angle, eSpace relativeTo = eSpace::Self);

		// Direction
		DirectX::XMFLOAT3 GetForward();
		DirectX::XMFLOAT3 GetUp();
		DirectX::XMFLOAT3 GetRight();

		// LookAt

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

	public:
		DirectX::XMFLOAT3 m_LocalPosition	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 m_LocalRotation	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 m_LocalScale		= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	private:
		Transform* m_pParent = nullptr;
		std::vector<Transform*> m_Children;
	};
}