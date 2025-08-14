#pragma once
#include "core/Object.h"

namespace Dive
{
	class GameObject;
	class Transform;

	enum class eComponentType
	{
		Undefined,
		Transform,
		Camera,
		Light,
		MeshRenderer,
		SkinnedMeshRender
	};

	class Component : public Object
	{
	public:
		Component(GameObject* gameObject);
		virtual ~Component() = default;

		std::string GetName() const override;

		virtual void Update() {}

		GameObject* GetGameObject() const;
		Transform* GetTransform() const;

		static constexpr eComponentType GetType() { return eComponentType::Undefined; }

	protected:
		GameObject* m_gameObject;
	};
}
