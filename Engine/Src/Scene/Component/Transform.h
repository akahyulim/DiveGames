#pragma once
#include "Component.h"

namespace Dive
{
	class Context;

	enum class eSpace
	{
		World,
		Self
	};

	class Transform : public Component
	{
		DIVE_OBJECT(Transform, Component)

	public:
		explicit Transform(Context* pContext);
		~Transform() override;

		void Clear();

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		void SetPosition(const DirectX::XMFLOAT3& pos);
		void SetPosition(float xPos, float yPos, float zPos);
		void SetLocalPosition(const DirectX::XMFLOAT3& pos) { m_LocalPosition = pos; }
		void SetLocalPosition(float xPos, float yPos, float zPos);

		DirectX::XMFLOAT3 GetRotation() const;
		DirectX::XMFLOAT4 GetRotationQuaternion() const;
		DirectX::XMFLOAT3 GetLocalRotation() const;
		DirectX::XMFLOAT4 GetLocalRotationQuaternion() const { return m_LocalRotation; }
		void SetRotation(const DirectX::XMFLOAT3& angle);
		void SetRotation(float xAngle, float yAngle, float zAngle);
		void SetRotation(const DirectX::XMFLOAT4& quaternion);
		void SetLocalRotation(const DirectX::XMFLOAT3& angle);
		void SetLocalRotation(float xAngle, float yAngle, float zAngle);
		void SetLocalRotation(const DirectX::XMFLOAT4& quaternion) { m_LocalRotation = quaternion; }

		DirectX::XMFLOAT3 GetScale() const;
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetScale(float xScale, float yScale, float zScale);
		void SetLocalScale(const DirectX::XMFLOAT3& scale) { m_LocalScale = scale; }
		void SetLocalScale(float xScale, float yScale, float zScale);

		DirectX::XMMATRIX GetMatrix();
		DirectX::XMFLOAT4X4 GetWorldTransform();
		DirectX::XMMATRIX GetLocalMatrix() const;
		void SetLocalMatrix(const DirectX::XMFLOAT4X4& mat);

		void Translate(DirectX::XMFLOAT3 translation, eSpace relativeTo = eSpace::Self);
		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);
		void Translate(DirectX::XMFLOAT3 translation, Transform relativeTo);
		void Translate(float x, float y, float z, Transform relativeTo);

		void Rotate(DirectX::XMFLOAT3 eulerAngle, eSpace relativeTo = eSpace::Self);
		void Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo = eSpace::Self);
		void Rotate(DirectX::XMFLOAT3 axis, float angle, eSpace relativeTo = eSpace::Self);

		void RotateAround(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 axis, float angle);

		void LookAt(Transform target, DirectX::XMFLOAT3 worldUp = {0.0f, 1.0f, 0.0f});
		void LookAt(DirectX::XMFLOAT3 worldPosition, DirectX::XMFLOAT3 worldUp = { 0.0f, 1.0f, 0.0f });

		DirectX::XMFLOAT3 GetForward() const;
		DirectX::XMFLOAT3 GetUp() const;
		DirectX::XMFLOAT3 GetRight() const;

		bool HasParent() const { return m_pParent; }
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);
		Transform* GetRoot();
		bool HasChild() const { return !m_Children.empty(); }
 		uint32_t GetChildCount() const { return static_cast<uint32_t>(m_Children.size()); }
		Transform* GetChild(uint32_t index) const;
		bool IsChildOf(Transform* pParent) const;
		void DetachChildren();
		std::vector<Transform*> GetChildren() const;

	private:
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Matrix;

		Transform* m_pParent;
		std::vector<Transform*> m_Children;
	};
}
