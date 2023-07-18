#pragma once
#include "Core/Object.h"
#include "yaml-cpp/yaml.h"

namespace Dive
{
	class GameObject;

	class Component
	{
	public:
		Component(GameObject* pGameObject);
		virtual ~Component() = default;

		uint64_t GetID() const { return m_ID; }
		void SetID(uint64_t id) { m_ID = id; }

		std::string GetName() const;

		virtual bool LoadFromFile(const std::string& filePath) { return true; }
		virtual bool SaveToFile(const std::string& filePath) { return true; }

		virtual bool SaveToYAML(YAML::Emitter& out) { return true; }

		virtual void Update(float delta) {}

		GameObject* GetGameObject() const { return m_pGameObject; }

	protected:
		uint64_t m_ID;
		GameObject* m_pGameObject;
	};
}