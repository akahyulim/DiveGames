#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Scene;

	class GameObject : public Object
	{
		DIVE_OBJECT(GameObject, Object)

	public:
		explicit GameObject(Context* pContext);
		~GameObject();

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		unsigned int GetID() const { return m_ID; }
		void SetID(unsigned int id) { m_ID = id; }

	private:
	private:
		Scene* m_pScene;
		std::string m_Name;
		unsigned int m_ID;
	};
}