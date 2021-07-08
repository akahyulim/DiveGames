#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include <string>
#include <vector>

namespace dive
{
	class GameObject;
	class FileStream;

	class Transform : public Component
	{
	public:
		Transform(GameObject* gameObject);
		~Transform();

		// 오버라이드 함수
		void Update(float deltaTime) override;
		void Serialize(FileStream* fileStream) override;
		void Deserialize(FileStream* fileStream) override;

		// 변환 함수
		void UpdateTransform();

		// Position
		DirectX::XMVECTOR GetPositionVector() const;
		DirectX::XMFLOAT3 GetPositionFloat3() const;
		void GetPosition(float& outX, float& outY, float& outZ) const;
		DirectX::XMVECTOR GetLocalPositionVector() const;
		DirectX::XMFLOAT3 GetLocalPositionFloat3() const { return m_LocalPosition; }
		void GetLocalPosition(float& outX, float& outY, float& outZ) const;
		void SetPositionByVector(const DirectX::FXMVECTOR& position);
		void SetPositionByFloat3(const DirectX::XMFLOAT3& position);
		void SetPosition(float x, float y, float z);
		void SetLocalPositionByVector(const DirectX::FXMVECTOR& position);
		void SetLocalPositionByFloat3(const DirectX::XMFLOAT3& position);
		void SetLocalPosition(float x, float y, float z);

		// Rotation
		DirectX::XMFLOAT4 GetRotation();
		DirectX::XMFLOAT4 GetLocalRotation() const { return m_LocalRotation; }
		void SetRotation(const DirectX::XMFLOAT3& rotation);
		void SetLocalRotation(const DirectX::XMFLOAT3& rotation);

		// Scale
		DirectX::XMFLOAT3 GetScale();
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);

		void SetLookAtByFloat3(const DirectX::XMFLOAT3& lookAt) { m_LookAt = lookAt; }
		void SetLookAt(float x, float y, float z);

		DirectX::XMMATRIX GetMatrix() const { return DirectX::XMLoadFloat4x4(&m_Matrix); }
		const DirectX::XMFLOAT4X4& GetMatrixFloat4x4() const { return m_Matrix; }
		DirectX::XMMATRIX GetLocalMatrix() const { return DirectX::XMLoadFloat4x4(&m_LocalMatrix); }
		const DirectX::XMFLOAT4X4& GetLocalMatrixFloat4x4() const { return m_LocalMatrix; }

		// Hierarchy
		Transform* GetRoot();
		bool HasParent() const { return (m_Parent != nullptr); }
		Transform* GetParent() const { return m_Parent; }
		void SetParent(Transform* parent);
		void BecomeOrphan();
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_Children.size()); }
		Transform* GetChildByName(const std::string& name) const;
		std::vector<Transform*> GetChildren() { return m_Children; }
		bool IsChildOf(const Transform* parent) const;
		void DetachChildren();
		void AcquireChidren();

	private:

	private:
		// transform
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Matrix;
		DirectX::XMFLOAT4X4 m_LocalMatrix;

		DirectX::XMFLOAT3 m_LookAt;

		// heirarchy
		Transform* m_Parent = nullptr;
		std::vector<Transform*> m_Children;
	};
}