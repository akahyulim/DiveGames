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
		Transform(GameObject* owner);
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
		DirectX::XMFLOAT3 GetLocalPositionFloat3() const { return mLocalPosition; }
		void GetLocalPosition(float& outX, float& outY, float& outZ) const;
		void SetPositionByVector(const DirectX::FXMVECTOR& position);
		void SetPositionByFloat3(const DirectX::XMFLOAT3& position);
		void SetPosition(float x, float y, float z);
		void SetLocalPositionByVector(const DirectX::FXMVECTOR& position);
		void SetLocalPositionByFloat3(const DirectX::XMFLOAT3& position);
		void SetLocalPosition(float x, float y, float z);

		// Rotation
		DirectX::XMFLOAT4 GetRotation();
		DirectX::XMFLOAT4 GetLocalRotation() const { return mLocalRotation; }
		void SetRotation(const DirectX::XMFLOAT3& rotation);
		void SetLocalRotation(const DirectX::XMFLOAT3& rotation);

		// Scale
		DirectX::XMFLOAT3 GetScale();
		DirectX::XMFLOAT3 GetLocalScale() const { return mLocalScale; }
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);

		void SetLookAtByFloat3(const DirectX::XMFLOAT3& lookAt) { mLookAt = lookAt; }
		void SetLookAt(float x, float y, float z);

		DirectX::XMMATRIX GetMatrix() const { return DirectX::XMLoadFloat4x4(&mMatrix); }
		const DirectX::XMFLOAT4X4& GetMatrixFloat4x4() const { return mMatrix; }
		DirectX::XMMATRIX GetLocalMatrix() const { return DirectX::XMLoadFloat4x4(&mLocalMatrix); }
		const DirectX::XMFLOAT4X4& GetLocalMatrixFloat4x4() const { return mLocalMatrix; }

		// Hierarchy
		Transform* GetRoot();
		bool HasParent() const { return (mParent != nullptr); }
		Transform* GetParent() const { return mParent; }
		void SetParent(Transform* parentTransform);
		void BecomeOrphan();
		unsigned int GetChildCount() const { return static_cast<unsigned int>(mChildren.size()); }
		Transform* Find(const std::string& name) const;
		Transform* GetChild(unsigned int index) const;
		std::vector<Transform*> GetChildren() { return mChildren; }
		bool IsChildOf(const Transform* parentTransform) const;
		void DetachChildren();
		void AcquireChidren();

	private:

	private:
		// transform
		DirectX::XMFLOAT3 mLocalPosition;
		DirectX::XMFLOAT4 mLocalRotation;
		DirectX::XMFLOAT3 mLocalScale;

		DirectX::XMFLOAT4X4 mMatrix;
		DirectX::XMFLOAT4X4 mLocalMatrix;

		DirectX::XMFLOAT3 mLookAt;

		// heirarchy
		Transform* mParent = nullptr;
		std::vector<Transform*> mChildren;
	};
}