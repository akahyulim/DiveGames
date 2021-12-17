#pragma once
#include "../../Core/Object.h"
#include <functional>
#include <any>
#include <vector>

namespace DiveEngine
{
	class GameObject;
	class FileStream;
	class Transform;

	enum class eComponentType
	{
		Unknown,
		Transform,
		Camera,
		Light,
		MeshRenderer,
		dvMeshRenderer,
		StaticMeshRenderer,
	};

	struct Attribute
	{
		std::function<std::any()> getter;
		std::function<void(std::any)> setter;
	};

	// base다.
	class Component : public Object
	{
	public:
		Component(GameObject* pGameObject, Transform* pTransform = nullptr);
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
		virtual void Serialize(FileStream* pFileStream) {}
		virtual void Deserialize(FileStream* pFileStream) {}

		GameObject* GetGameObject() const { return m_pGameObject; }
		// pGameObject를 바꾸려 시도할 수 있다.=> 없을 거 같은데...
		void SetGameObject(GameObject* pGameObject) { m_pGameObject = pGameObject; }

		eComponentType GetType() const { return m_Type; }

		Transform* GetTransform() { return m_pTransform; }

		template<class T>
		static constexpr eComponentType TypeToEnum();

		const std::vector<Attribute>& GetAttributes() const { return m_Attributes; }
		void SetAttributes(const std::vector<Attribute>& attributes)
		{
			for (size_t i = 0; i != attributes.size(); i++)
			{
				m_Attributes[i].setter(attributes[i].getter());
			}
		}

	protected:
		#define REGISTER_ATTRIBUTE_GET_SET(getter, setter, type) RegisterAttribute(     \
        [this]()                        { return getter(); },                           \
        [this](const std::any& valueIn) { setter(std::any_cast<type>(valueIn)); });     \

		#define REGISTER_ATTRIBUTE_VALUE_SET(value, setter, type) RegisterAttribute(    \
        [this]()                        { return value; },                              \
        [this](const std::any& valueIn) { setter(std::any_cast<type>(valueIn)); });     \

		#define REGISTER_ATTRIBUTE_VALUE_VALUE(value, type) RegisterAttribute(          \
        [this]()                        { return value; },                              \
        [this](const std::any& valueIn) { value = std::any_cast<type>(valueIn); });     \

		// Registers an attribute
		void RegisterAttribute(std::function<std::any()>&& getter, std::function<void(std::any)>&& setter)
		{
			Attribute attribute;
			attribute.getter = std::move(getter);
			attribute.setter = std::move(setter);
			m_Attributes.emplace_back(attribute);
		}

	protected:
		GameObject* m_pGameObject = nullptr;
		Transform* m_pTransform = nullptr;

		eComponentType m_Type = eComponentType::Unknown;

		std::vector<Attribute> m_Attributes;
	};
}