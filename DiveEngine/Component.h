#pragma once
#include "Object.h"

namespace Dive
{
	class GameObject;
	class FileStream;

	// base다.
	class Component : public Object
	{
	public:
		Component(size_t typeHash) : Object(typeHash) {}
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
		virtual void Serialize(FileStream* pStream) {}
		virtual void Deserialize(FileStream* pStream) {}

		GameObject* GetOwner() const { return m_pOwner; }
		// owner를 바꾸려 시도할 수 있다.
		void SetOwnder(GameObject* pOwner) { m_pOwner = pOwner; }

	private:
		GameObject* m_pOwner = nullptr;
	};
}