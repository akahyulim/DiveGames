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

		void Update(float delta) override;

		void Clear();

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMVECTOR GetPositionVector() const;
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		DirectX::XMVECTOR GetLocalPositionVector() const;
		void SetPosition(float x, float y, float z);
		void SetPosition(const DirectX::XMFLOAT3& pos);
		void SetPositionVector(const DirectX::XMVECTOR& pos);
		void SetLocalPosition(float x, float y, float z);
		void SetLocalPosition(const DirectX::XMFLOAT3& pos);
		void SetLocalPositionVector(const DirectX::XMVECTOR& pos);

		DirectX::XMFLOAT3 GetRotation() const;
		DirectX::XMFLOAT4 GetRotationQuaternion() const;
		DirectX::XMVECTOR GetRotationQuaternionVector() const;
		DirectX::XMFLOAT3 GetLocalRotation() const;
		DirectX::XMFLOAT4 GetLocalRotationQuaternion() const { return m_LocalRotation; }
		DirectX::XMVECTOR GetLocalRotationQuaternionVector() const;
		void SetRotation(float xAngle, float yAngle, float zAngle);
		void SetRotation(const DirectX::XMFLOAT3& angle);
		void SetRotationQuaternion(const DirectX::XMFLOAT4& quaternion);
		void SetRotationQuaternionVector(const DirectX::XMVECTOR& quaternion);
		void SetLocalRotation(float xAngle, float yAngle, float zAngle);
		void SetLocalRotation(const DirectX::XMFLOAT3& angle);
		void SetLocalRotationQuaternion(const DirectX::XMFLOAT4& quaternion);
		void SetLocalRotationQuaternionVector(const DirectX::XMVECTOR& quaternion);

		DirectX::XMFLOAT3 GetScale() const;
		DirectX::XMVECTOR GetScaleVector() const;
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		DirectX::XMVECTOR GetLocalScaleVector() const;
		void SetScale(float x, float y, float z);
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetScaleVector(const DirectX::XMVECTOR& scale);
		void SetLocalScale(float x, float y, float z);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);
		void SetLocalScaleVector(const DirectX::XMVECTOR& scale);

		DirectX::XMMATRIX GetMatrix() const;
		DirectX::XMMATRIX GetLocalMatrix() const;

		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);
		void Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo = eSpace::Self);
		void TranslateVector(const DirectX::XMVECTOR& translation, eSpace relativeTo = eSpace::Self);
		void Translate(const DirectX::XMFLOAT3& translation, Transform relativeTo);

		void Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo = eSpace::Self);
		void Rotate(const DirectX::XMFLOAT3& eulerAngle, eSpace relativeTo = eSpace::Self);
		void RotateQuaternion(const DirectX::XMFLOAT4& delta, eSpace relativeTo = eSpace::Self);
		void RotateQuaternionVector(const DirectX::XMVECTOR& delta, eSpace relativeTo = eSpace::Self);
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
		void updateTransform();

	private:
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Matrix;
		DirectX::XMFLOAT4X4 m_LocalMatrix;

		Transform* m_pParent;
		std::vector<Transform*> m_Children;
	};
}
