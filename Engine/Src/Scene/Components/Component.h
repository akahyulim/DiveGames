#pragma once
#include "Core/TypeInfo.h"

namespace Dive
{
	class GameObject;
	class Transform;

	class Component
	{
	public:
		Component(GameObject* pGameObject);
		virtual ~Component() = default;

		virtual void Update() {}

		std::string GetName() const;

		GameObject* GetGameObject() const;
		Transform* GetTransform();

		virtual size_t GetTypeHash() const = 0;
		virtual const std::string& GetTypeName() const = 0;
		virtual const TypeInfo* GetTypeInfo() const = 0;
		static const TypeInfo* GetTypeInfoStatic() { return nullptr; }

	protected:
		GameObject* m_pGameObject;
		Transform* m_pTransform;
	};
}