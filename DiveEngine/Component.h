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
		Component(size_t typeHash, GameObject* owner, Transform* transform = nullptr);
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
		virtual void Serialize(FileStream* fileStream) {}
		virtual void Deserialize(FileStream* fileStream) {}

		GameObject* GetOwner() const { return mOwner; }
		// owner를 바꾸려 시도할 수 있다.=> 없을 거 같은데...
		void SetOwnder(GameObject* owner) { mOwner = owner; }

		Transform* GetTransform() { return mTransform; }

	private:
		GameObject* mOwner = nullptr;
		Transform* mTransform = nullptr;
	};
}