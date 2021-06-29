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
		~GameObject() = default;

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

		bool GetActive() const { return mbActive; }
		void SetActive(bool active) { mbActive = active; }

		unsigned int GetComponentCount() const { return static_cast<unsigned int>(mComponents.size()); }

	private:
	private:
		// ����Ƽ�� 'Object 1'�� �����Ѵ�.
		std::string mName = "Object";
		bool mbActive = true;

		std::vector<std::shared_ptr<Component>> mComponents;

		Transform* mTransform;

	};

	template<typename T>
	T* GameObject::AddComponent()
	{
		if (auto component = GetComponent<T>())
		{
			return component;
		}

		// �����ڸ� �����ؾ� �Ѵ�.
		auto component = std::make_shared<T>();
		mComponents.push_back(std::static_pointer_cast<Component>(component));
		// awake�� �ִٸ� ȣ��

		EVENT_FIRE(eEventType::SceneResolve);

		return component.get();
	}

	template<typename T>
	void GameObject::RemoveComponent()
	{
		auto& it = mComponents.begin();
		for (it; it != mComponents.end();)
		{
			if ((*it)->GetTypeHash() == typeid(T).hash_code())
			{
				// ���� �Լ� ȣ��?
				(*it).reset();
				it = mComponents.erase(it);
				return;
			}
			else
				it++;
		}

		// �̰� ���� Ȯ�� �ʿ�
		EVENT_FIRE(eEventType::SceneResolve);
	}

	template<typename T>
	T* GameObject::GetComponent()
	{
		for (auto& component : mComponents)
		{
			if (component->GetTypeHash() == typeid(T).hash_code())
			{
				return static_cast<T*>(component.get());
			}
		}

		return nullptr;
	}
}