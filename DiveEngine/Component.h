#pragma once
#include "Object.h"

namespace dive
{
	class GameObject;
	class FileStream;
	class Transform;

	// base다.
	class Component : public Object
	{
	public:
		Component(size_t typeHash, GameObject* gameObject, Transform* transform = nullptr);
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
		virtual void Serialize(FileStream* fileStream) {}
		virtual void Deserialize(FileStream* fileStream) {}

		GameObject* GetGameObject() const { return mGameObject; }
		// gameObject를 바꾸려 시도할 수 있다.=> 없을 거 같은데...
		void SetGameObject(GameObject* gameObject) { mGameObject = gameObject; }

		Transform* GetTransform() { return mTransform; }

	private:
		GameObject* mGameObject = nullptr;
		Transform* mTransform = nullptr;
	};
}