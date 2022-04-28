#pragma once
#include "Base/Base.h"
#include "Base/Object.h"

namespace Dive
{
	class GameObject;

	enum class eComponentType
	{
		Unknown,
		Transform,
		Camera,
		SpriteRenderable, MeshRenderable, SkinnedMeshRenderable,
		Light,
		Terrain,
		Particle,	// ����Ƽ������ ParicleSystem
		Skybox,
	};

#define COMPONENT_CLASS_TYPE(type)	static eComponentType GetStaticType() { return eComponentType::type; } \
									virtual eComponentType GetType() const override { return GetStaticType(); } \
									virtual const char* GetName() const override { return #type; }

	class Component : public Object
	{
	public:
		Component(GameObject* pGameObject);
		Component(GameObject* pGameObject, unsigned long long id);
		virtual ~Component() = default;

		// �ϴ� �ƹ��� ������� �ʴ´�.
		virtual void Update(float delta) {}

		GameObject* GetGameObject() { return m_pGameObject; }

		virtual eComponentType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string GetStr() const { return GetName(); }

	protected:
		GameObject* m_pGameObject	= nullptr;
	};


	// ����Ƽ�� ��� Terrain, Skybox, Light�� Behaviour�� ��ӹ޴´�.
	// ������Ʈ ������ �Ǵ��ϴ� �ǵ�, ���� ���� ������ �������� �𸣰ڴ�. �ϴ� ���� ����.
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