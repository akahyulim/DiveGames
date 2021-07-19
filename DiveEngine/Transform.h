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
		DirectX::XMVECTOR GetRotationVector() const;
		DirectX::XMFLOAT4 GetRotationFloat4() const;
		void GetRotation(float& outX, float& outY, float& outZ) const;
		DirectX::XMVECTOR GetLocalRotationVector() const;
		DirectX::XMFLOAT4 GetLocalRotationFloat4() const { return m_LocalRotation; }
		void GetLocalRotation(float& outX, float& outY, float& outZ) const;
		void SetRotationByVector(const DirectX::FXMVECTOR& rotation);
		void SetRotationByFloat4(const DirectX::XMFLOAT4& rotation);
		void SetRotation(float x, float y, float z);
		void SetLocalRotationByVector(const DirectX::FXMVECTOR& rotation);
		void SetLocalRotationByFloat4(const DirectX::XMFLOAT4& rotation);
		void SetLocalRotation(float x, float y, float z);

		// Scale
		DirectX::XMVECTOR GetScaleVector() const;
		DirectX::XMFLOAT3 GetScaleFloat3() const;
		void GetScale(float& outX, float& outY, float& outZ);
		DirectX::XMVECTOR GetLocalScaleVector() const;
		DirectX::XMFLOAT3 GetLocalScaleFloat3() const { return m_LocalScale; }
		void GetLocalScale(float& outX, float& outY, float& outZ);
		void SetScaleVector(const DirectX::XMVECTOR& scale);
		void SetScaleFloat3(const DirectX::XMFLOAT3& scale);
		void SetScale(float x, float y, float z);
		void SetLocalScaleVector(const DirectX::XMVECTOR& scale);
		void SetLocalScaleFloat3(const DirectX::XMFLOAT3& scale);
		void SetLocalScale(float x, float y, float z);

		void SetLookAtByFloat3(const DirectX::XMFLOAT3& lookAt) { m_LookAt = lookAt; }
		void SetLookAt(float x, float y, float z);

		DirectX::XMMATRIX GetMatrix() const { return DirectX::XMLoadFloat4x4(&m_Matrix); }
		const DirectX::XMFLOAT4X4& GetMatrixFloat4x4() const { return m_Matrix; }
		DirectX::XMMATRIX GetLocalMatrix() const { return DirectX::XMLoadFloat4x4(&m_LocalMatrix); }
		const DirectX::XMFLOAT4X4& GetLocalMatrixFloat4x4() const { return m_LocalMatrix; }

		// unity
		bool HasChanged() const { return m_bChanged; }

		// Hierarchy
		std::vector<Transform*> GetChildren() const { return m_Children; }
		Transform* GetRoot();
		bool HasParent() const { return (m_Parent != nullptr); }
		Transform* GetParent() const { return m_Parent; }
		void SetParent(Transform* parent);
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_Children.size()); }
		Transform* GetChildByName(const std::string& name) const;
		bool IsChildOf(const Transform* parent) const;
		void DetachChildren();


	private:

	private:
		bool m_bChanged = false;

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