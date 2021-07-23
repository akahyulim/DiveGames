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

		// 오버라이드 함수
		void Update(float deltaTime) override;
		void Serialize(FileStream* pFileStream) override;
		void Deserialize(FileStream* pFileStream) override;

		// 변환 함수
		void UpdateTransform();

		// Position
		DirectX::XMVECTOR GetPosition() const;
		void GetPosition(float& outX, float& outY, float& outZ) const;
		void SetPosition(const DirectX::FXMVECTOR& position);
		void SetPosition(float x, float y, float z);
		DirectX::XMVECTOR GetLocalPosition() const ;
		void GetLocalPosition(float& outX, float& outY, float& outZ) const;
		void SetLocalPosition(const DirectX::FXMVECTOR& position);
		void SetLocalPosition(float x, float y, float z);
	
		// Rotation
		DirectX::XMVECTOR GetRotation() const;
		void GetRotation(float& degreeX, float& degreeY, float& degreeZ) const;
		void SetRotation(const DirectX::FXMVECTOR& quaternion);
		void SetRotation(float degreeX, float degreeY, float degreeZ);
		DirectX::XMVECTOR GetLocalRotation() const;
		void GetLocalRotation(float& degreeX, float& degreeY, float& degreeZ) const;
		void SetLocalRotation(const DirectX::FXMVECTOR& quaternion);
		void SetLocalRotation(float degreeX, float degreeY, float degreeZ);
		
		// Scale
		DirectX::XMVECTOR GetScale() const;
		void GetScale(float& outX, float& outY, float& outZ) const;
		void SetScale(const DirectX::FXMVECTOR& scale);
		void SetScale(float x, float y, float z);
		DirectX::XMVECTOR GetLocalScale() const;
		void GetLocalScale(float& outX, float& outY, float& outZ) const;
		void SetLocalScale(const DirectX::FXMVECTOR& scale);
		void SetLocalScale(float x, float y, float z);

		// Move
		void Translate(const DirectX::FXMVECTOR& translation, eSpace relativeTo = eSpace::Self);
		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);
		void Rotate(const DirectX::FXMVECTOR& quaternion, eSpace relativeTo = eSpace::Self);
		void Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo = eSpace::Self);
		// void RotateAround

		void LookAt(const Transform& target);
		void LookAt(const DirectX::FXMVECTOR& worldPosition);
		void LookAt(float posX, float posY, float posZ);

		// forward, up ,right

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
		
	private:
		bool m_bChanged = false;

		// Transform
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Matrix;
		DirectX::XMFLOAT4X4 m_LocalMatrix;

		// heirarchy
		Transform* m_pParent = nullptr;
		std::vector<Transform*> m_Children;
	};
}