#pragma once
#include "Base/Base.h"

namespace Dive
{
	class GameObject;

	enum class eComponentType
	{
		Unknown,
		Transform,
		Camera,
		SpriteRenderer,
	};

// ���⿡ base type�� �߰��ϸ� �� �� ������...
#define COMPONENT_CLASS_TYPE(type)	static eComponentType GetStaticType() { return eComponentType::type; } \
									virtual eComponentType GetType() const override { return GetStaticType(); } \
									virtual const char* GetName() const override { return #type; }

	class Component
	{
	public:
		Component(GameObject* pGameObject) : m_pGameObject(pGameObject) {}
		virtual ~Component() = default;

		GameObject* GetGameObject() { return m_pGameObject; }

		virtual eComponentType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string GetStr() const { return GetName(); }
 
		// �ڽ��� �Ҽӵ� GameObject�� IsntanceID�� ����
		unsigned long long GetInstanceID() const;

	protected:
		GameObject* m_pGameObject	= nullptr;
	};
}