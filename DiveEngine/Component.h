#pragma once
#include "Object.h"

namespace Dive
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
		virtual void Serialize(FileStream* pStream) {}
		virtual void Deserialize(FileStream* pStream) {}

		GameObject* GetOwner() const { return m_pOwner; }
		// owner�� �ٲٷ� �õ��� �� �ִ�.
		void SetOwnder(GameObject* pOwner) { m_pOwner = pOwner; }

	private:
		GameObject* m_pOwner = nullptr;
	};
}