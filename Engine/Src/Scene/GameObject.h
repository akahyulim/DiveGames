#pragma once
#include "Component/Transform.h"

namespace Dive
{
	class Scene;

	class GameObject
	{
	public:
		GameObject(Scene* pScene);
		~GameObject() = default;

		template<class T>
		T* AddComponent()
		{
			// T�� Component�� ����� ������ Ȯ���� �ʿ��ϴ�.

			if (HasComponent<T>())
				return GetComponent<T>();

			return new T(this);
		}
		
		template<class T>
		void RemoveComponent()
		{

		}

		template<class T>
		bool HasComponent()
		{
			return false;
		}

		template<class T>
		T* GetComponent()
		{
			return nullptr;
		}

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
	private:
		std::string m_Name = "GameObject";

		Scene* m_pScene			= nullptr;
	};
}