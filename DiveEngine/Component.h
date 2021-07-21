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
		Component(size_t typeHash, GameObject* pGameObject, Transform* pTransform = nullptr);
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
		virtual void Serialize(FileStream* pFileStream) {}
		virtual void Deserialize(FileStream* pFileStream) {}

		GameObject* GetGameObject() const { return m_pGameObject; }
		// pGameObject를 바꾸려 시도할 수 있다.=> 없을 거 같은데...
		void SetGameObject(GameObject* pGameObject) { m_pGameObject = pGameObject; }

		Transform* GetTransform() { return m_pTransform; }

	private:
		GameObject* m_pGameObject = nullptr;
		Transform* m_pTransform = nullptr;
	};
}