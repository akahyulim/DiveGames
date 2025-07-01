#pragma once
#include "core/Object.h"

namespace Dive
{
	class GameObject;

	enum class eComponentType
	{
		Undefined,
		Transform,
		Camera,
		MeshRenderer,
		SkinnedMeshRenderer
	};

	class Component : public Object
	{
	public:
		Component(GameObject* gameObject);
		virtual ~Component() = default;

		std::string GetName() override;

		virtual void Update() {}

		GameObject* GetGameObject() const;

		static constexpr eComponentType GetType() { return eComponentType::Undefined; }

	protected:
		GameObject* m_GameObject;
	};
}