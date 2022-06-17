#pragma once
#include "Base/Base.h"
#include "Base/Object.h"

namespace Dive
{
	class GameObject;

	enum class eComponentType
	{
		Transform,
		Camera,
		SpriteRenderable, MeshRenderable, SkinnedMeshRenderable,
		Light,
		Count
	};

#define COMPONENT_CLASS_TYPE(type)	\
	public:	\
	static eComponentType GetStaticType() { return eComponentType::type; } \
	virtual eComponentType GetType() const override { return GetStaticType(); } \
	virtual const char* GetTypeStr() const override { return #type; }

	class Component
	{
	public:
		Component(GameObject* pGameObject);
		virtual ~Component() = default;

		// 일단 아무도 사용하지 않는다.
		virtual void Update(float delta) {}

		GameObject* GetGameObject() { return m_pGameObject; }

		virtual eComponentType GetType() const = 0;
		virtual const char* GetTypeStr() const = 0;

		uint64_t GetInstanceID() const;
		std::string GetName() const;

	protected:
		GameObject* m_pGameObject = nullptr;
	};


	// 유니티의 경우 Terrain, Skybox, Light는 Behaviour을 상속받는다.
	// 업데이트 유무를 판단하는 건데, 굳이 나눈 이유는 아직까지 모르겠다. 일단 만들어만 놓자.
	class Behaviour : public Component
	{
	public:
		Behaviour(GameObject* pGameObject) : Component(pGameObject) {}
		virtual ~Behaviour() = default;

		bool IsEnabled() const { return m_bEnabled; }
		void SetEnabled(bool enable) { m_bEnabled = enable; }

	private:
		bool m_bEnabled = true;
	};
}