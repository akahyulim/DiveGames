#pragma once
#include "DvComponent.h"

namespace Dive
{
	class DvGameObject;

	class DvTransform : public DvComponent
	{
	public:
		enum class eSpace
		{
			World,
			Local
		};

	public:
		DvTransform(DvGameObject* pGameObject);
		~DvTransform() = default;

		void Update() override;

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMVECTOR GetPositionVector() const;
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		DirectX::XMVECTOR GetLocalPositionVector() const { return DirectX::XMLoadFloat3(&m_LocalPosition); }
		void SetPosition(float posX, float posY, float posZ);
		void SetPosition(const DirectX::XMFLOAT3& pos);
		void SetPosition(const DirectX::XMVECTOR& pos);
		void SetLocalPosition(float posX, float posY, float posZ);
		void SetLocalPosition(const DirectX::XMFLOAT3& pos);
		void SetLocalPosition(const DirectX::XMVECTOR& pos);

		DirectX::XMFLOAT4 GetRotationQuaternion() const;
		DirectX::XMVECTOR GetRotationQuaternionVector() const;
		DirectX::XMFLOAT3 GetRotationDegree() const;
		DirectX::XMFLOAT4 GetLocalRotationQuaternion() const { return m_LocalRotation; }
		DirectX::XMVECTOR GetLocalRotationQuaternionVector() const { return XMLoadFloat4(&m_LocalRotation); }
		DirectX::XMFLOAT3 GetLocalRotationDegree() const;
		void SetRotation(float degreeX, float degreeY, float degreeZ);
		void SetRotation(const DirectX::XMFLOAT3& degree);
		void SetRotation(const DirectX::XMFLOAT4& quaternion);
		void SetRotation(const DirectX::XMVECTOR& quaternion);
		void SetLocalRotation(float degreeX, float degreeY, float degreeZ);
		void SetLocalRotation(const DirectX::XMFLOAT3& degree);
		void SetLocalRotation(const DirectX::XMFLOAT4& quaternion);
		void SetLocalRotation(const DirectX::XMVECTOR& quaternion);

		DirectX::XMFLOAT3 GetScale() const;
		DirectX::XMVECTOR GetScaleVector() const;
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		DirectX::XMVECTOR GetLocalScaleVector() const { return XMLoadFloat3(&m_LocalScale); }
		void SetScale(float scaleX, float scaleY, float scaleZ);
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetScale(const DirectX::XMVECTOR& scale);
		void SetLocalScale(float scaleX, float scaleY, float scaleZ);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);
		void SetLocalScale(const DirectX::XMVECTOR& scale);

		// 유니티는 LocalToWorldMatrix, WorldToLocalMatrix 두 개의 행렬만 공개해 놓았다.
		DirectX::XMMATRIX GetLocalToWorldMatrix() const;
		DirectX::XMMATRIX GetWorldToLocalMatrix() const;
		DirectX::XMMATRIX GetPositionMatrix() const;
		DirectX::XMMATRIX GetRotationMatrix() const;
		DirectX::XMMATRIX GetScaleMatrix() const;
		DirectX::XMMATRIX GetLocalPositionMatrix() const;
		DirectX::XMMATRIX GetLocalRotationMatrix() const;
		DirectX::XMMATRIX GetLocalScaleMatrix() const;
		DirectX::XMMATRIX GetWorldMatrix() const;
		DirectX::XMMATRIX GetLocalMatrix() const;
		void SetWorldMatrix(const DirectX::XMFLOAT4X4& mat);
		void SetWorldMatrix(const DirectX::XMMATRIX& mat);
		void SetLocalMatrix(const DirectX::XMFLOAT4X4& mat);
		void SetLocalMatrix(const DirectX::XMMATRIX& mat);

		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Local);
		void Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo = eSpace::Local);
		void Translate(const DirectX::XMVECTOR& translation, eSpace relativeTo = eSpace::Local);
		void Translate(const DirectX::XMFLOAT3& translation, DvTransform relativeTo);

		void Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo = eSpace::Local);
		void Rotate(const DirectX::XMFLOAT3& degree, eSpace relativeTo = eSpace::Local);
		void Rotate(const DirectX::XMFLOAT4& delta, eSpace relativeTo = eSpace::Local);
		void Rotate(const DirectX::XMVECTOR& delta, eSpace relativeTo = eSpace::Local);
		void Rotate(DirectX::XMFLOAT3 axis, float degree, eSpace relativeTo = eSpace::Local);

		void RotateAround(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 axis, float degree);

		DirectX::XMFLOAT3 GetLookAt();
		DirectX::XMVECTOR GetLookAtVector();
		void SetLookAt(const DvTransform& target, const DirectX::XMFLOAT3& worldUp = { 0.0f, 1.0f, 0.0f });
		void SetLookAt(float posX, float posY, float posZ, const DirectX::XMFLOAT3& worldUp = { 0.0f, 1.0f, 0.0f });
		void SetLookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& worldUp = { 0.0f, 1.0f, 0.0f });
		void SetLookAt(const DirectX::XMVECTOR& pos, const DirectX::XMFLOAT3& worldUp = { 0.0f, 1.0f, 0.0f });

		DirectX::XMFLOAT3 GetForward() const;
		DirectX::XMVECTOR GetForwardVector() const;
		DirectX::XMFLOAT3 GetUpward() const;
		DirectX::XMVECTOR GetUpwardVector() const;
		DirectX::XMFLOAT3 GetRightward() const;
		DirectX::XMVECTOR GetRightwardVector() const;

		bool HasParent() const;
		DvTransform* GetParent() const { return m_pParent; }
		void SetParent(DvTransform* pParent);
		DvTransform* GetRoot();
		bool HasChild() const { return !m_Children.empty(); }
		uint32_t GetChildCount() const { return static_cast<uint32_t>(m_Children.size()); }
		DvTransform* GetChild(uint32_t index) const;
		bool IsChildOf(DvTransform* pParent) const;
		void DetachChildren();
		std::vector<DvTransform*> GetChildren() const { return m_Children; }
		DvTransform* FindByName(const std::string& name);

	private:
		void updateTransform();

	private:
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_WorldMatrix;
		DirectX::XMFLOAT4X4 m_LocalMatrix;

		DvTransform* m_pParent;
		std::vector<DvTransform*> m_Children;
	};
}
