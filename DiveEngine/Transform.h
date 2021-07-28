#pragma once
#include "Component.h"
#include "GraphicsInclude.h"
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

		void Clear();

		// Position
		DirectX::XMVECTOR GetPositionVector() const;
		DirectX::XMFLOAT3 GetPosition() const;
		void GetPosition(float& outX, float& outY, float& outZ) const;
		void SetPositionVector(const DirectX::XMVECTOR& position);
		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetPosition(float x, float y, float z);
		DirectX::XMVECTOR GetLocalPositionVector() const ;
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		void GetLocalPosition(float& outX, float& outY, float& outZ) const;
		void SetLocalPositionVector(const DirectX::XMVECTOR& position);
		void SetLocalPosition(const DirectX::XMFLOAT3& position);
		void SetLocalPosition(float x, float y, float z);
	
		// Rotation
		DirectX::XMVECTOR GetRotationVector() const;
		DirectX::XMFLOAT4 GetRotation() const;
		DirectX::XMFLOAT3 GetRotationEulerAngles() const;
		void GetRotationEulerAngles(float& degreeX, float& degreeY, float& degreeZ) const;
		void SetRotationVector(const DirectX::XMVECTOR& quaternion);
		void SetRotation(const DirectX::XMFLOAT4& quaternion);
		void SetRotationEulerAngles(const DirectX::XMFLOAT3& eularAngles);
		void SetRotationEulerAngles(float degreeX, float degreeY, float degreeZ);
		DirectX::XMVECTOR GetLocalRotationVector() const;
		DirectX::XMFLOAT4 GetLocalRotation() const { return m_LocalRotation; }
		DirectX::XMFLOAT3 GetLocalRotationEulerAngles() const;
		void GetLocalRotationEulerAngles(float& degreeX, float& degreeY, float& degreeZ) const;
		void SetLocalRotationVector(const DirectX::XMVECTOR& quaternion);
		void SetLocalRotation(const DirectX::XMFLOAT4& quaternion);
		void SetLocalRotationEulerAngles(const DirectX::XMFLOAT3& eularAngles);
		void SetLocalRotationEulerAngles(float degreeX, float degreeY, float degreeZ);
		
		// Scale
		DirectX::XMVECTOR GetScaleVector() const;
		DirectX::XMFLOAT3 GetScale() const;
		void GetScale(float& outX, float& outY, float& outZ) const;
		void SetScaleVector(const DirectX::XMVECTOR& scale);
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetScale(float x, float y, float z);
		DirectX::XMVECTOR GetLocalScaleVector() const;
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		void GetLocalScale(float& outX, float& outY, float& outZ) const;
		void SetLocalScaleVector(const DirectX::XMVECTOR& scale);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);
		void SetLocalScale(float x, float y, float z);

		// Move
		void TranslateVector(const DirectX::XMVECTOR& translation, eSpace relativeTo = eSpace::Self);
		void Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo = eSpace::Self);
		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);
		void RotateVector(const DirectX::XMVECTOR& quaternion, eSpace relativeTo = eSpace::Self);
		void Rotate(const DirectX::XMFLOAT4& quaternion, eSpace relativeTo = eSpace::Self);
		void RotateEulerAngles(const DirectX::XMFLOAT3& eularAngles, eSpace relativeTo = eSpace::Self);
		void RotateEulerAngles(float degreeX, float degreeY, float degreeZ, eSpace relativeTo = eSpace::Self);
		// void RotateAround

		void LookAt(const Transform& target);
		void LookAt(const DirectX::XMVECTOR& worldPosition);
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
		void updateTransform();
		
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