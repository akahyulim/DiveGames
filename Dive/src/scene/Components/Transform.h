#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	class Transform : public Component
	{
	public:
		enum class eSpace
		{
			Self,
			Scene
		};

		DV_CLASS(Transform, Component);

	public:
		Transform(GameObject* pGameObject);
		~Transform() = default;

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMVECTOR GetPositionVector() const;
		void SetPosition(float posX, float posY, float posZ);
		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetPosition(const DirectX::XMVECTOR& position);

		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		DirectX::XMVECTOR GetLocalPositionVector() const;
		void SetLocalPosition(float posX, float posY, float posZ);
		void SetLocalPosition(const DirectX::XMFLOAT3& position);
		void SetLocalPosition(const DirectX::XMVECTOR& position);

		DirectX::XMFLOAT3 GetRotationDegrees() const;
		DirectX::XMFLOAT4 GetRotationQuaternion() const;
		DirectX::XMVECTOR GetRotationQuaternionVector() const;
		void SetRotation(float degreeX, float degreeY, float degreeZ);
		void SetRotation(const DirectX::XMFLOAT4& quaternion);
		void SetRotation(const DirectX::XMVECTOR& quaternion);

		DirectX::XMFLOAT3 GetLocalRotationDegrees() const;
		DirectX::XMFLOAT4 GetLocalRotationQuaternion() const { return m_LocalRotation; }
		DirectX::XMVECTOR GetLocalRotationQuaternionVector() const;
		void SetLocalRotation(float degreeX, float degreeY, float degreeZ);
		void SetLocalRotation(const DirectX::XMFLOAT4& quaternion);
		void SetLocalRotation(const DirectX::XMVECTOR& quaternion);

		DirectX::XMFLOAT3 GetScale() const;
		DirectX::XMVECTOR GetScaleVector() const;
		void SetScale(float sclX, float sclY, float sclZ);
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetScale(const DirectX::XMVECTOR& scale);

		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		DirectX::XMVECTOR GetLocalScaleVector() const;
		void SetLocalScale(float sclX, float sclY, float sclZ);
		void SetLocalScale(const DirectX::XMFLOAT3& scale);
		void SetLocalScale(const DirectX::XMVECTOR& scale);

		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);

		void Rotate(const DirectX::XMVECTOR& quaternion, eSpace relativeTo = eSpace::Self);
		void Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo = eSpace::Self);

		void LookAt(float x, float y, float z, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f);
		void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });
		void LookAt(const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& up = { 0.0f, 1.0f, 0.0f, 1.0f });
		void LookAt(const Transform* pTarget, const DirectX::XMVECTOR& up = { 0.0f, 1.0f, 0.0f, 1.0f });

		DirectX::XMFLOAT4X4 GetSceneTransform() const { return m_Transform; }	// 추후 이름에서 Scene 제거
		void SetTransform(const DirectX::XMFLOAT4X4& world);
		DirectX::XMMATRIX GetMatrix() const { return DirectX::XMLoadFloat4x4(&m_Transform); }
		void SetMatrix(const DirectX::XMMATRIX& world);
		DirectX::XMFLOAT4X4 GetLocalTransform() const { return m_LocalTransform; }
		void SetLocalTransform(const DirectX::XMFLOAT4X4& local);
		DirectX::XMMATRIX GetLocalMatrix() const { return DirectX::XMLoadFloat4x4(&m_LocalTransform); }
		void SetLocalMatrix(const DirectX::XMMATRIX& local);

		DirectX::XMFLOAT3 GetForward() const;
		DirectX::XMVECTOR GetForwardVector() const;
		DirectX::XMFLOAT3 GetUpward() const;
		DirectX::XMVECTOR GetUpwardVector() const;
		DirectX::XMFLOAT3 GetRightward() const;
		DirectX::XMVECTOR GetRightwardVector() const;

		// hiarachy
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);

		const uint64_t GetParentID() const { return m_ParentID; }
		void SetParentID(const uint64_t id) { m_ParentID = id; }	// 좀 애매하다. 사용자는 이로인해 계층구조가 생성된다 여길 수 있다.

		bool IsRoot() const { return !m_pParent; }
		Transform* GetRoot();
		bool HasChild() const { return !m_Children.empty(); }
		uint32_t GetChildCount() const { return static_cast<uint32_t>(m_Children.size()); }
		Transform* GetChild(uint32_t index) const;
		bool IsChildOf(Transform* pParent) const;
		void DetachChildren();
		std::vector<Transform*> GetChildren() const { return m_Children; }

	private:
		void updateTransform();

	private:
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;
		 
		DirectX::XMFLOAT4X4 m_Transform;
		DirectX::XMFLOAT4X4 m_LocalTransform;

		Transform* m_pParent;
		std::vector<Transform*> m_Children;

		uint64_t m_ParentID;
	};
}
