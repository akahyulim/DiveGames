#pragma once
#include "Object.h"

namespace dive
{
	class GameObject;
	class FileStream;

	// base��.
	class Component : public Object
	{
	public:
		Component(size_t typeHash) : Object(typeHash) {}
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
		virtual void Serialize(FileStream* fileStream) {}
		virtual void Deserialize(FileStream* fileStream) {}

		GameObject* GetOwner() const { return mOwner; }
		// owner�� �ٲٷ� �õ��� �� �ִ�.
		void SetOwnder(GameObject* owner) { mOwner = owner; }

	private:
		GameObject* mOwner = nullptr;
	};
}