#pragma once
#include "Core/Object.h"
#include "yaml-cpp/yaml.h"

namespace Dive
{
	class GameObject;

	class Component : public Object
	{
	public:
		Component(GameObject* pGameObject);
		~Component() = default;

		virtual bool LoadFromFile(const std::string& filePath) { return true; }
		virtual bool SaveToFile(const std::string& filePath) { return true; }

		virtual bool SaveToYAML(YAML::Emitter& out) { return true; }

		virtual void Update(float delta) {}

		GameObject* GetGameObject() const { return m_pGameObject; }

	protected:
		GameObject* m_pGameObject;
	};
}