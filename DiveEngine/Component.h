#pragma once
#include "Object.h"

namespace dive
{
	class GameObject;
	class FileStream;
	class Transform;

	enum class eComponentType
	{
		Unknown,
		Transform,
		Camera,
		Light,
		MeshRenderer,
	};

	// base다.
	class Component : public Object
	{
	public:
		Component(GameObject* pGameObject, Transform* pTransform = nullptr);
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
		virtual void Serialize(FileStream* pFileStream) {}
		virtual void Deserialize(FileStream* pFileStream) {}

		GameObject* GetGameObject() const { return m_pGameObject; }
		// pGameObject를 바꾸려 시도할 수 있다.=> 없을 거 같은데...
		void SetGameObject(GameObject* pGameObject) { m_pGameObject = pGameObject; }

		eComponentType GetType() const { return m_Type; }

		Transform* GetTransform() { return m_pTransform; }

		template<class T>
		static constexpr eComponentType TypeToEnum();

	protected:
		GameObject* m_pGameObject = nullptr;
		Transform* m_pTransform = nullptr;

		eComponentType m_Type = eComponentType::Unknown;
	};
}