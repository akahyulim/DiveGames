#pragma once
#include "Object.h"

namespace dive
{
	class GameObject;
	class FileStream;
	class Transform;

	// base��.
	class Component : public Object
	{
	public:
		Component(size_t typeHash, GameObject* owner, Transform* transform = nullptr);
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
		virtual void Serialize(FileStream* fileStream) {}
		virtual void Deserialize(FileStream* fileStream) {}

		GameObject* GetOwner() const { return mOwner; }
		// owner�� �ٲٷ� �õ��� �� �ִ�.=> ���� �� ������...
		void SetOwnder(GameObject* owner) { mOwner = owner; }

		Transform* GetTransform() { return mTransform; }

	private:
		GameObject* mOwner = nullptr;
		Transform* mTransform = nullptr;
	};
}