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
		// Position, Scale�� �޶�� �Ѵ�.
		// Quaternion, eulerAngles, Radian���� �ٷ����� �����̴�.
		// �ʹ� ����. ����Ƽ�� ���� �ٷ�����?
		// ����Ƽ�� �ϴ� Quaternion���� world, local rotation�� ������.
		// �׸��� Rotate �Լ������� Vector3�� eulerAngles�� �޴´�.


		// Scale
		DirectX::XMFLOAT3 GetScale() const;
		void SetScale(const DirectX::XMFLOAT3& scale);
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		void SetLocalScale(const DirectX::XMFLOAT3& scale);

		// translate
		// rotate

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
		DirectX::XMVECTOR getPositionByVector() const;
		void setPositionByVector(const DirectX::FXMVECTOR& position);
		DirectX::XMVECTOR getLocalPositionByVector() const;
		void setLocalPositionByVector(const DirectX::FXMVECTOR& position);

		DirectX::XMVECTOR getScaleByVector() const;
		void setScaleByVector(const DirectX::FXMVECTOR& scale);
		DirectX::XMVECTOR getLocalScaleByVector() const;
		void setLocalScaleByVector(const DirectX::FXMVECTOR& scale);
		
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