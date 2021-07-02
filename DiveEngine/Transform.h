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

		// �������̵� �Լ�
		void Update(float deltaTime) override;
		void Serialize(FileStream* fileStream) override;
		void Deserialize(FileStream* fileStream) override;

		// ��ȯ �Լ�
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

		// �������� �Լ� ===================================================================================
		// ������ �����Ѵ�.
		Transform* GetRoot();
		// �θ� ���� ���θ� �����Ѵ�.
		bool HasParent() const { return (mParent != nullptr); }
		// �θ� �����͸� �����Ѵ�.
		Transform* GetParent() const { return mParent; }
		void SetParent(Transform* parentTransform);
		// �θ� �����Ѵٸ� �����Ѵ�.
		void BecomeOrphan();
		// �ڽ��� ���� �����Ѵ�.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(mChildren.size()); }
		// �̸����� �ڽ��� ã�� �����Ѵ�.
		Transform* Find(const std::string& name) const;
		// �ε����� �ڽ��� ã�� �����Ѵ�.
		Transform* GetChild(unsigned int index) const;
		// ��� �ڽ��� �����Ѵ�.
		std::vector<Transform*> GetChildren() { return mChildren; }
		// �ڽ��� parentTransform�� �ڽ����� Ȯ���Ѵ�.
		bool IsChildOf(const Transform* parentTransform) const;
		// ��� �ڽ��� �����. �ڽĵ��� �������� �ʰ� �θ� ���ְ� ���� �� ����Ѵ�.
		void DetachChildren();
		// �ڽĵ�κ��� �θ� �ڽ� ���踦 ��������� �籸���Ѵ�.
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