#pragma once
#include "Core/Object.h"
#include "Scene.h"

namespace Dive
{
	class Scene;
	class Component;

	enum class eSpace
	{
		Self,
		World
	};

	class GameObject
	{
	public:
		GameObject(Scene* pScene, const std::string& name = "GameObject");
		~GameObject();

		void Update();

		void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }
		
		void SetID(uint64_t id) { m_ID = id; }
		uint64_t GetID() const { return m_ID; }

		bool IsActive() const { return m_bActive; }
		void SetActive(bool active) { m_bActive = active; }

		void MarkRemoveTarget() { m_bMarkedTarget = true; }
		bool IsRemovedTarget() const { return m_bMarkedTarget; }

		template<class T> T* AddComponent();
		template<class T> bool AddComponent(T* pComponent);

		template<class T> bool HasComponent();
		bool HashComponent(size_t typeHash) const;
		
		template<class T> T* GetComponent() const;
		Component* GetComponent(size_t typeHash) const;

		// transform
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
		void LookAt(const GameObject* pTarget, const DirectX::XMVECTOR& up = { 0.0f, 1.0f, 0.0f, 1.0f });

		DirectX::XMFLOAT4X4 GetWorldTransform() const { return m_Transform; }	// 추후 이름에서 Scene 제거
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
		GameObject* GetParent() const { return m_pParent; }
		void SetParent(GameObject* pParent);
		bool IsRoot() const { return !m_pParent; }
		GameObject* GetRoot();
		bool HasChild() const { return !m_Children.empty(); }
		uint32_t GetChildCount() const { return static_cast<uint32_t>(m_Children.size()); }
		GameObject* GetChild(uint32_t index) const;
		bool IsChildOf(GameObject* pParent) const;
		void DetachChildren();
		std::vector<GameObject*> GetChildren() const { return m_Children; }

	private:
		void updateTransform();

	private:
		std::string m_Name;
		uint64_t m_ID;

		Scene* m_pScene;
		bool m_bActive;
		bool m_bMarkedTarget;
		std::unordered_map<size_t, Component*> m_Components;

		// transform
		DirectX::XMFLOAT3 m_LocalPosition;
		DirectX::XMFLOAT4 m_LocalRotation;
		DirectX::XMFLOAT3 m_LocalScale;

		DirectX::XMFLOAT4X4 m_Transform;
		DirectX::XMFLOAT4X4 m_LocalTransform;

		GameObject* m_pParent;
		std::vector<GameObject*> m_Children;
	};

	template<class T>
	T* GameObject::AddComponent()
	{
		auto pExisted = GetComponent<T>();
		if (pExisted)
			return pExisted;

		auto pNewComponent = new T(this);
		m_Components[T::GetTypeHashStatic()] = static_cast<Component*>(pNewComponent);

		return pNewComponent;
	}

	template<class T> 
	bool GameObject::AddComponent(T* pComponent)
	{
		if (!pComponent)
			return false;

		auto pExisted = GetComponent<T>();
		if (pExisted)
			return false;

		m_Components[T::GetTypeHashStatic()] = static_cast<Component*>(pComponent);
		pComponent->SetGameObject(this);
		return true;
	}

	template<class T>
	bool GameObject::HasComponent()
	{
		auto it = m_Components.find(T::GetTypeHashStatic());
		return it != m_Components.end();
	}

	template<class T>
	T* GameObject::GetComponent() const
	{
		auto it = m_Components.find(T::GetTypeHashStatic());
		if (it != m_Components.end())
			return dynamic_cast<T*>(it->second);

		return nullptr;
	}
}