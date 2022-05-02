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
		Terrain,
		Particle,	// ����Ƽ������ ParicleSystem
		Skybox,
		Unknown,	// �̰� �ϴ� Base���� ����ϴ� ���� �ִ�.
		Count
	};

#define COMPONENT_CLASS_TYPE(type)	static eComponentType GetStaticType() { return eComponentType::type; } \
									virtual eComponentType GetType() const override { return GetStaticType(); } \
									virtual const char* GetTypeStr() const override { return #type; }

	class Component : public Object
	{
	public:
		Component(GameObject* pGameObject, unsigned long long id = 0);
		virtual ~Component() = default;

		// �ϴ� �ƹ��� ������� �ʴ´�.
		virtual void Update(float delta) {}

		GameObject* GetGameObject() { return m_pGameObject; }

		virtual eComponentType GetType() const = 0;
		virtual const char* GetTypeStr() const = 0;

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