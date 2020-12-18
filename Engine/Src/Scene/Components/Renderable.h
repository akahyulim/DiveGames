#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class GameObject;
	class Model;
	class Material;

	class Renderable : public Component
	{
	public:
		explicit Renderable(Context* context, GameObject* owner);
		~Renderable();

		const std::shared_ptr<Model>& GetModel() const { return m_model; }
		const std::shared_ptr<Material>& GetMaterial() const { return m_material; }

		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

	private:
	private:
		bool m_bEnabled;

		std::shared_ptr<Model> m_model;
		std::shared_ptr<Material> m_material;
	};
}