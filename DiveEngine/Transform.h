#pragma once
#include "Component.h"
#include "GraphicsInclude.h"
#include <string>
#include <vector>

namespace dive
{
	class FileStream;

	class Transform : public Component
	{
	public:
		Transform();
		~Transform();

		// 오버라이드 함수
		void Update(float deltaTime) override;
		void Serialize(FileStream* fileStream) override;
		void Deserialize(FileStream* fileStream) override;

		// 변환 함수
		void UpdateTransform();

		DirectX::XMFLOAT3 GetPosition() const;
		void GetPosition(float& outX, float& outY, float& outZ);
		DirectX::XMFLOAT3 GetLocalPosition() const { return mLocalPosition; }
		void GetLocalPosition(float& outX, float& outY, float& outZ);
		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetPosition(float x, float y, float z);
		void SetLocalPosition(const DirectX::XMFLOAT3& position);
		void SetLocalPosition(float x, float y, float z);

		DirectX::XMFLOAT4 GetRotation();
		DirectX::XMFLOAT4 GetLocalRotation() const { return mLocalRotation; }
		void SetRotation(const DirectX::XMFLOAT3& rotation);
		void SetLocalRotation(const DirectX::XMFLOAT3& rotation);

		DirectX::XMFLOAT3 GetScale();
		DirectX::XMFLOAT3 GetLocalScale() const { return mLocalScale; }
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);

		void SetLookAt(const DirectX::XMFLOAT3& lookAt) { mLookAt = lookAt; }
		const DirectX::XMFLOAT4X4& GetMatrix() const { return mMatrix; }
		const DirectX::XMFLOAT4X4& GetLocalMatrix() const { return mLocalMatrix; }

		// 계층구조 함수 ===================================================================================
		// 시조를 리턴한다.
		Transform* GetRoot();
		// 부모 존재 여부를 리턴한다.
		bool HasParent() const { return (mParent != nullptr); }
		// 부모 포인터를 리턴한다.
		Transform* GetParent() const { return mParent; }
		void SetParent(Transform* parentTransform);
		// 부모가 존재한다면 독립한다.
		void BecomeOrphan();
		// 자식의 수를 리턴한다.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(mChildren.size()); }
		// 이름으로 자식을 찾아 리턴한다.
		Transform* Find(const std::string& name) const;
		// 인덱스로 자식을 찾아 리턴한다.
		Transform* GetChild(unsigned int index) const;
		// 모든 자식을 리턴한다.
		std::vector<Transform*> GetChildren() { return mChildren; }
		// 자신이 parentTransform의 자식인지 확인한다.
		bool IsChildOf(const Transform* parentTransform) const;
		// 모든 자식을 떼어낸다. 자식들을 제거하지 않고 부모를 없애고 싶을 때 사용한다.
		void DetachChildren();
		// 자식들로부터 부모 자식 관계를 재귀적으로 재구성한다.
		void AcquireChidren();

	private:
		DirectX::XMVECTOR getPosition() const;
		DirectX::XMVECTOR getLocalPosition() const;
		void setPosition(const DirectX::XMVECTOR& position);
		void setLocalPosition(const DirectX::XMVECTOR& position);

		DirectX::XMMATRIX getMatrix() const;
		DirectX::XMMATRIX getLocalMatrix() const;

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