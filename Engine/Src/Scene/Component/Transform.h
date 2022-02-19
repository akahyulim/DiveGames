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

		// Position
		DirectX::XMFLOAT3 GetLocalPosition() const;
		DirectX::XMVECTOR GetLocalPositionVector() const;
		void GetLocalPosition(float& outX, float& outY, float& outZ) const;
		void SetLocalPosition(const DirectX::XMFLOAT3& position);
		void SetLocalPosition(const DirectX::XMVECTOR& position);
		void SetLocalPosition(float x, float y, float z);
		DirectX::XMFLOAT3 GetPosition();
		DirectX::XMVECTOR GetPositionVector();
		void GetPosition(float& outX, float& outY, float& outZ);
		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetPosition(const DirectX::XMVECTOR& position);
		void SetPosition(float x, float y, float z);

		// Rotation
		DirectX::XMVECTOR GetLocalRotationVector() const;

		// Scale
		DirectX::XMFLOAT3 GetLocalScale() const;
		DirectX::XMVECTOR GetLocalScaleVector() const;
		void GetLocalScale(float& outX, float& outY, float& outZ);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);
		void SetLcoalScale(const DirectX::XMVECTOR& scale);
		void SetLocalScale(float x, float y, float z);
		DirectX::XMFLOAT3 GetScale();
		DirectX::XMVECTOR GetScaleVector();
		void GetScale(float& outX, float& outY, float& outZ);
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetScale(const DirectX::XMVECTOR& scale);
		void SetScale(float x, float y, float z);

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

		// Matrix
		DirectX::XMFLOAT4X4 GetLocalMatrix4X4();
		DirectX::XMMATRIX GetLocalMatrix();
		DirectX::XMFLOAT4X4 GetMatrix4X4();
		DirectX::XMMATRIX GetMatrix();

		// Hierarchy
		bool HasParent() const { return (m_pParent != nullptr); }
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);
		Transform* GetRoot();
		std::vector<Transform*> GetChildren() const;
		unsigned int GetChildCount();
		Transform* GetChild(int index);
		bool IsChildOf(const Transform* pParent);
		void DetachChildren();

	private:
	private:
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Matrix;
		DirectX::XMFLOAT4X4 m_LocalMatrix;

		Transform* m_pParent = nullptr;
		std::vector<Transform*> m_Children;
	};
}