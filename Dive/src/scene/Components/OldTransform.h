#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	
	enum class eSpace
	{
		World,
		Self
	};

	class OldTransform : public Component
	{
	public:
		OldTransform();
		OldTransform(GameObject* pGameObject = nullptr);
		~OldTransform() override;

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		void Update() override;

		void Clear();

		DirectX::XMVECTOR GetPosition() const;
		void GetPosition(DirectX::XMFLOAT3& pos) const;
		DirectX::XMVECTOR GetLocalPosition() const;
		void GetLocalPosition(DirectX::XMFLOAT3& localPos) const;
		void SetPosition(float x, float y, float z);
		void SetPosition(const DirectX::XMFLOAT3& pos);
		void SetPosition(const DirectX::XMVECTOR& pos);
		void SetLocalPosition(float x, float y, float z);
		void SetLocalPosition(const DirectX::XMFLOAT3& pos);
		void SetLocalPosition(const DirectX::XMVECTOR& pos);

		DirectX::XMVECTOR GetRotation() const;
		DirectX::XMVECTOR GetLocalRotation() const;
		DirectX::XMFLOAT3 GetLocalRotationDegrees() const;
		void SetRotation(float xAngle, float yAngle, float zAngle);
		void SetRotation(const DirectX::XMFLOAT3& angle);
		void SetRotationQuaternion(const DirectX::XMFLOAT4& quaternion);
		void SetRotationQuaternion(const DirectX::XMVECTOR& quaternion);
		void SetLocalRotation(float xAngle, float yAngle, float zAngle);
		void SetLocalRotation(const DirectX::XMFLOAT3& angle);
		void SetLocalRotationQuaternion(const DirectX::XMFLOAT4& quaternion);
		void SetLocalRotationQuaternion(const DirectX::XMVECTOR& quaternion);

		DirectX::XMVECTOR GetScale() const;
		DirectX::XMVECTOR GetLocalScale() const;
		void SetScale(float x, float y, float z);
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetScale(const DirectX::XMVECTOR& scale);
		void SetLocalScale(float x, float y, float z);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);
		void SetLocalScale(const DirectX::XMVECTOR& scale);

		// 유니티는 LocalToWorldMatrix, WorldToLocalMatrix 두 개의 행렬만 공개해 놓았다.
		DirectX::XMMATRIX GetMatrix() const;
		void SetMatrix(const DirectX::XMFLOAT4X4& matrix);
		DirectX::XMMATRIX GetLocalMatrix() const;
		void SetLocalMatrix(const DirectX::XMFLOAT4X4& matrix);

		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);
		void Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo = eSpace::Self);
		void Translate(const DirectX::XMVECTOR& translation, eSpace relativeTo = eSpace::Self);
		void Translate(const DirectX::XMFLOAT3& translation, OldTransform relativeTo);

		void Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo = eSpace::Self);
		void Rotate(const DirectX::XMFLOAT3& eulerAngle, eSpace relativeTo = eSpace::Self);
		void RotateQuaternion(const DirectX::XMFLOAT4& delta, eSpace relativeTo = eSpace::Self);
		void RotateQuaternion(const DirectX::XMVECTOR& delta, eSpace relativeTo = eSpace::Self);
		void Rotate(DirectX::XMFLOAT3 axis, float angle, eSpace relativeTo = eSpace::Self);

		void RotateAround(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 axis, float angle);

		DirectX::XMVECTOR GetLookAt();
		void LookAt(const OldTransform& target);
		void LookAt(const DirectX::XMFLOAT3& worldPosition);

		DirectX::XMVECTOR GetForward() const;
		DirectX::XMVECTOR GetUp() const;
		DirectX::XMVECTOR GetRight() const;

		bool HasParent() const;
		OldTransform* GetParent() const { return m_pParent; }
		void SetParent(OldTransform* pParent);
		OldTransform* GetRoot();
		bool HasChild() const { return !m_Children.empty(); }
		uint32_t GetChildCount() const { return static_cast<uint32_t>(m_Children.size()); }
		OldTransform* GetChild(uint32_t index) const;
		bool IsChildOf(OldTransform* pParent) const;
		void DetachChildren();
		std::vector<OldTransform*> GetChildren() const { return m_Children; }

		OldTransform* FindByName(const std::string& name);

	private:
		void updateTransform();

	private:
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Matrix;
		DirectX::XMFLOAT4X4 m_LocalMatrix;

		DirectX::XMFLOAT3 m_LookAt;

		OldTransform* m_pParent;
		std::vector<OldTransform*> m_Children;
	};
}
