#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include <string>
#include <vector>

namespace dive
{
	class GameObject;
	class FileStream;

	enum class eSpace
	{
		World,
		Self,
	};

	class Transform : public Component
	{
	public:
		Transform(GameObject* pGameObject);
		~Transform();

		// �������̵� �Լ�
		void Update(float deltaTime) override;
		void Serialize(FileStream* pFileStream) override;
		void Deserialize(FileStream* pFileStream) override;

		// ��ȯ �Լ�
		void UpdateTransform();

		// Position
		DirectX::XMFLOAT3 GetPosition() const;
		void SetPosition(const DirectX::XMFLOAT3& position);
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		void SetLocalPosition(const DirectX::XMFLOAT3& position);
		
		// Rotation
		DirectX::XMFLOAT4 GetRotation() const;
		void SetRotation(const DirectX::XMFLOAT4& rotation);
		DirectX::XMFLOAT4 GetLocalRotation() const { return m_LocalRotation; }
		void SetLocalRotation(const DirectX::XMFLOAT4& rotation);
		// eular angle�� �޴� �͵鵵 �ʿ���....��?
		// ����Ƽ�� ��� Quaternion�� Eular �Լ��� �־ ��ȯ���� �ش�...
		void SetRotation(float xAngle, float yAngle, float zAngle);

		// Scale
		DirectX::XMFLOAT3 GetScale() const;
		void SetScale(const DirectX::XMFLOAT3& scale);
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		void SetLocalScale(const DirectX::XMFLOAT3& scale);

		void Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo = eSpace::Self);
		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);

		void Rotate(const DirectX::XMFLOAT3& enulerAngles);
		void Rotate(float xAngle, float yAngle, float zAngle);

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
		bool HasParent() const { return (m_pParent != nullptr); }
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_Children.size()); }
		Transform* GetChildByName(const std::string& name) const;
		bool IsChildOf(const Transform* pParent) const;
		void DetachChildren();


	private:
		// ����... ��׸� �ٽ� public���� �÷��� �� �� ����.
		// deltaTime�� ���Ҷ��� VECTOR�� �� ���ϱ� �����̴�.
		DirectX::XMVECTOR getPosition() const;
		void setPosition(const DirectX::FXMVECTOR& position);
		DirectX::XMVECTOR getLocalPosition() const;
		void setLocalPosition(const DirectX::FXMVECTOR& position);

		DirectX::XMVECTOR getRotation() const;
		void setRotation(const DirectX::FXMVECTOR& rotation);
		DirectX::XMVECTOR getLocalRotation() const;
		void setLocalRotation(const DirectX::FXMVECTOR& rotation);

		DirectX::XMVECTOR getScale() const;
		void setScale(const DirectX::FXMVECTOR& scale);
		DirectX::XMVECTOR getLocalScale() const;
		void setLocalScale(const DirectX::FXMVECTOR& scale);

		void translate(const DirectX::FXMVECTOR& translation, eSpace relativeTo);
		
	private:
		bool m_bChanged = false;

		// pTransform
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Matrix;
		DirectX::XMFLOAT4X4 m_LocalMatrix;

		DirectX::XMFLOAT3 m_LookAt;

		// heirarchy
		Transform* m_pParent = nullptr;
		std::vector<Transform*> m_Children;
	};
}