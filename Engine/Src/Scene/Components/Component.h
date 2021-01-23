#pragma once
#include "DivePch.h"
#include "Core/Object.h"


namespace Dive
{
	class Context;
	class FileStream;
	class GameObject;

	enum class eComponentType : unsigned int
	{
		Transform,
		MeshFilter,
		MeshRenderer,
		Renderable,
		Camera,
	//	Unknown
	};

	class Component : public Object
	{
		DIVE_OBJECT(Component, Object);

	public:
		explicit Component(Context* context, GameObject* owner);
		virtual ~Component() = default;

		// serialization
		virtual void Serialize(FileStream& stream) {}
		virtual void Deserialize(FileStream& stream) {}

		virtual void OnAwake() {}
		virtual void OnStart() {}

		virtual void OnStop() {}
		virtual void OnRemove() {}

		virtual void OnUpdate() {}

		virtual void Copy(Component* other) {}

		GameObject* GetOwner()		const { return m_owner; }
		eComponentType GetType()	const { return m_type; }
		std::string GetName()		const;

	protected:

	protected:
		GameObject* m_owner;
		eComponentType m_type;
	};
}