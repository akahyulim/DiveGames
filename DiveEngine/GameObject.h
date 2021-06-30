#pragma once
#include "Object.h"
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Event.h"
#include <string>
#include <vector>
#include <memory>

namespace dive
{
	class FileStream;

	class GameObject : public Object
	{
	public:
		GameObject();
		~GameObject();

		void Serialize(FileStream* fileStream);
		void Deserialize(FileStream* fileStream, Transform* parentTransform = nullptr);

		void Update(float deltaTime);

		template<typename T>
		T* AddComponent();
		Component* AddComponent(unsigned int typeHash, unsigned int id = 0);

		template<typename T>
		void RemoveComponent();

		template<typename T>
		T* GetComponent();

		Transform* GetTransform() { return mTransform; }

		const std::string& GetName() const { return mName; }
		void SetName(const std::string name) { mName = name; }

		bool IsActive() const { return mbActive; }
		void SetActive(bool active) { mbActive = active; }

		unsigned int GetComponentCount() const { return static_cast<unsigned int>(mComponents.size()); }

		// 이름을 바꾸자
		void MarkForDestruction() { mbDestructionPending = true; }
		bool IsPendingDestruction() const { return mbDestructionPending; }

	private:
	private:
		// 유니티는 'Object 1'로 시작한다.
		std::string mName = "Object";
		bool mbActive = true;

		std::vector<Component*> mComponents;

		Transform* mTransform;

		// 이름을 바꾸자
		bool mbDestructionPending = false;

	};

	template<typename T>
	T* GameObject::AddComponent()
	{
		if (auto component = GetComponent<T>())
		{
			return component;
		}

		auto newComponent = dynamic_cast<T*>(mComponents.emplace_back(static_cast<Component*>(new T)));
		assert(newComponent);

		// awake가 있다면 호출

		EVENT_FIRE(eEventType::SceneResolve);

		return newComponent;
	}

	template<typename T>
	void GameObject::RemoveComponent()
	{
		auto& it = mComponents.begin();
		for (it; it != mComponents.end();)
		{
			if ((*it)->GetTypeHash() == typeid(T).hash_code())
			{
				// 제거 함수 호출?
				(*it).reset();
				it = mComponents.erase(it);
				return;
			}
			else
				it++;
		}

		// 이건 추후 확인 필요
		EVENT_FIRE(eEventType::SceneResolve);
	}

	template<typename T>
	T* GameObject::GetComponent()
	{
		for (auto component : mComponents)
		{
			if (component->GetTypeHash() == typeid(T).hash_code())
			{
				return dynamic_cast<T*>(component);
			}
		}

		return nullptr;
	}
}