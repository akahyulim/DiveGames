#pragma once

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

	class Component
	{
	public:
		Component(GameObject* gameObject) : m_GameObject(gameObject) {}
		virtual ~Component() = default;

		GameObject* GetGameObject() const { return m_GameObject; }

		static constexpr eComponentType GetType() { return eComponentType::Undefined; }

	protected:
		GameObject* m_GameObject = nullptr;
	};
}