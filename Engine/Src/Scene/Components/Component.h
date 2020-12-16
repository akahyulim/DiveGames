#pragma once
#include "DivePch.h"
#include "Core/Object.h"
#include "Scene/GameObject.h"


namespace Dive
{
	class Context;

	enum class eComponentType
	{
		Transform,
		Renderable,
	};

	class Component : public Object
	{
		DIVE_OBJECT(Component, Object);

	public:
		explicit Component(Context* context, GameObject* owner);
		virtual ~Component() = default;

		// serialization

		virtual void OnAwake() {}
		virtual void OnStart() {}

		virtual void OnStop() {}
		virtual void OnRemove() {}

		virtual void OnUpdate() {}

		GameObject* GetOwner()		const { return m_owner; }
		eComponentType GetType()	const { return m_type; }
		unsigned int GetID()		const { return m_owner->GetID(); }
		std::string GetName()		const { return m_owner->GetName(); }

	protected:

	protected:
		GameObject* m_owner;
		eComponentType m_type;
	};
}